#include "chunk.h"
#include "textures.h"
#include "tile.h"
#include "common.h"

int Chunk_rebuiltThisFrame = 0;
int Chunk_updates = 0;

static Tesselator* sharedTesselator = NULL;

Chunk* Chunk_create(Level* level, int x0, int y0, int z0, int x1, int y1, int z1) {
    if (!sharedTesselator) {
        sharedTesselator = Tesselator_create();
    }

    Chunk* chunk = (Chunk*)malloc(sizeof(Chunk));
    if (chunk) {
        chunk->level = level;
        chunk->x0 = x0;
        chunk->y0 = y0;
        chunk->z0 = z0;
        chunk->x1 = x1;
        chunk->y1 = y1;
        chunk->z1 = z1;
        chunk->aabb = AABB_create((float)x0, (float)y0, (float)z0, (float)x1, (float)y1, (float)z1);
        chunk->dirty = true;
        chunk->lists = glGenLists(2);
    }
    return chunk;
}

void Chunk_destroy(Chunk* self) {
    if (self) {
        glDeleteLists(self->lists, 2);
        free(self);
    }
}

static void Chunk_rebuild(Chunk* self, int layer) {
    if (Chunk_rebuiltThisFrame != 2) {
        self->dirty = false;
        Chunk_updates++;
        Chunk_rebuiltThisFrame++;

        int id = Textures_loadTexture("/terrain.png", GL_NEAREST);
        glNewList(self->lists + layer, GL_COMPILE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, id);
        Tesselator_init(sharedTesselator);

        int tiles = 0;
        for (int x = self->x0; x < self->x1; x++) {
            for (int y = self->y0; y < self->y1; y++) {
                for (int z = self->z0; z < self->z1; z++) {
                    if (Level_isTile(self->level, x, y, z)) {
                        int tex = (y == self->level->depth * 2 / 3) ? 0 : 1;
                        tiles++;
                        if (tex == 0) {
                            Tile_render(&Tile_rock, sharedTesselator, self->level, layer, x, y, z);
                        } else {
                            Tile_render(&Tile_grass, sharedTesselator, self->level, layer, x, y, z);
                        }
                    }
                }
            }
        }

        Tesselator_flush(sharedTesselator);
        glDisable(GL_TEXTURE_2D);
        glEndList();
    }
}

void Chunk_render(Chunk* self, int layer) {
    if (!self) return;
    if (self->dirty) {
        Chunk_rebuild(self, 0);
        Chunk_rebuild(self, 1);
    }
    glCallList(self->lists + layer);
}

void Chunk_setDirty(Chunk* self) {
    if (self) {
        self->dirty = true;
    }
}
