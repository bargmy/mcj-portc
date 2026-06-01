#include "level_renderer.h"
#include "frustum.h"
#include "tile.h"
#include "common.h"

static void tileChanged_cb(void* obj, int x, int y, int z) {
    LevelRenderer* self = (LevelRenderer*)obj;
    LevelRenderer_setDirty(self, x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}

static void lightColumnChanged_cb(void* obj, int x, int z, int y0, int y1) {
    LevelRenderer* self = (LevelRenderer*)obj;
    LevelRenderer_setDirty(self, x - 1, y0 - 1, z - 1, x + 1, y1 + 1, z + 1);
}

static void allChanged_cb(void* obj) {
    LevelRenderer* self = (LevelRenderer*)obj;
    LevelRenderer_setDirty(self, 0, 0, 0, self->level->width, self->level->depth, self->level->height);
}

LevelRenderer* LevelRenderer_create(Level* level) {
    LevelRenderer* lr = (LevelRenderer*)malloc(sizeof(LevelRenderer));
    if (lr) {
        lr->level = level;
        lr->xChunks = level->width / 16;
        lr->yChunks = level->depth / 16;
        lr->zChunks = level->height / 16;
        lr->chunkCount = lr->xChunks * lr->yChunks * lr->zChunks;
        lr->chunks = (Chunk**)malloc(lr->chunkCount * sizeof(Chunk*));
        lr->t = Tesselator_create();

        // Wire up listener callbacks
        LevelListener listener;
        listener.tileChanged = tileChanged_cb;
        listener.lightColumnChanged = lightColumnChanged_cb;
        listener.allChanged = allChanged_cb;
        listener.obj = lr;
        Level_addListener(level, listener);

        for (int x = 0; x < lr->xChunks; x++) {
            for (int y = 0; y < lr->yChunks; y++) {
                for (int z = 0; z < lr->zChunks; z++) {
                    int x0 = x * 16;
                    int y0 = y * 16;
                    int z0 = z * 16;
                    int x1 = (x + 1) * 16;
                    int y1 = (y + 1) * 16;
                    int z1 = (z + 1) * 16;

                    if (x1 > level->width) x1 = level->width;
                    if (y1 > level->depth) y1 = level->depth;
                    if (z1 > level->height) z1 = level->height;

                    int idx = (x + y * lr->xChunks) * lr->zChunks + z;
                    lr->chunks[idx] = Chunk_create(level, x0, y0, z0, x1, y1, z1);
                }
            }
        }
    }
    return lr;
}

void LevelRenderer_destroy(LevelRenderer* self) {
    if (self) {
        // Remove listener
        LevelListener listener;
        listener.obj = self;
        Level_removeListener(self->level, listener);

        for (int i = 0; i < self->chunkCount; i++) {
            Chunk_destroy(self->chunks[i]);
        }
        free(self->chunks);
        Tesselator_destroy(self->t);
        free(self);
    }
}

void LevelRenderer_render(LevelRenderer* self, Player* player, int layer) {
    if (!self) return;
    Chunk_rebuiltThisFrame = 0;
    Frustum* frustum = Frustum_getFrustum();

    for (int i = 0; i < self->chunkCount; i++) {
        if (Frustum_cubeInFrustumAABB(frustum, self->chunks[i]->aabb)) {
            Chunk_render(self->chunks[i], layer);
        }
    }
}

void LevelRenderer_pick(LevelRenderer* self, Player* player) {
    if (!self || !player) return;
    float r = 3.0f;
    AABB box = AABB_grow(player->base.bb, r, r, r);
    int x0 = (int)box.x0;
    int x1 = (int)(box.x1 + 1.0f);
    int y0 = (int)box.y0;
    int y1 = (int)(box.y1 + 1.0f);
    int z0 = (int)box.z0;
    int z1 = (int)(box.z1 + 1.0f);

    glInitNames();

    for (int x = x0; x < x1; x++) {
        glPushName(x);

        for (int y = y0; y < y1; y++) {
            glPushName(y);

            for (int z = z0; z < z1; z++) {
                glPushName(z);

                if (Level_isSolidTile(self->level, x, y, z)) {
                    glPushName(0);

                    for (int i = 0; i < 6; i++) {
                        int nx = x, ny = y, nz = z;
                        if (i == 0) ny--;
                        if (i == 1) ny++;
                        if (i == 2) nz--;
                        if (i == 3) nz++;
                        if (i == 4) nx--;
                        if (i == 5) nx++;

                        if (!Level_isSolidTile(self->level, nx, ny, nz)) {
                            glPushName(i);
                            Tesselator_init(self->t);
                            Tile_renderFace(&Tile_rock, self->t, x, y, z, i);
                            Tesselator_flush(self->t);
                            glPopName();
                        }
                    }

                    glPopName();
                }

                glPopName();
            }

            glPopName();
        }

        glPopName();
    }
}

void LevelRenderer_renderHit(LevelRenderer* self, HitResult h) {
    if (!self) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.0f, 1.0f, 1.0f, sinf((float)glfwGetTime() * 10.0f) * 0.2f + 0.4f);
    Tesselator_init(self->t);
    Tile_renderFace(&Tile_rock, self->t, h.x, h.y, h.z, h.f);
    Tesselator_flush(self->t);
    glDisable(GL_BLEND);
}

void LevelRenderer_setDirty(LevelRenderer* self, int x0, int y0, int z0, int x1, int y1, int z1) {
    if (!self) return;
    x0 /= 16;
    x1 /= 16;
    y0 /= 16;
    y1 /= 16;
    z0 /= 16;
    z1 /= 16;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 >= self->xChunks) x1 = self->xChunks - 1;
    if (y1 >= self->yChunks) y1 = self->yChunks - 1;
    if (z1 >= self->zChunks) z1 = self->zChunks - 1;

    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            for (int z = z0; z <= z1; z++) {
                int idx = (x + y * self->xChunks) * self->zChunks + z;
                Chunk_setDirty(self->chunks[idx]);
            }
        }
    }
}
