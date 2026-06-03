#include "level.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

Level* Level_create(int w, int h, int d) {
    Level* lvl = (Level*)malloc(sizeof(Level));
    if (lvl) {
        lvl->width = w;
        lvl->height = h;
        lvl->depth = d;
        lvl->blocks = (unsigned char*)malloc(w * h * d * sizeof(unsigned char));
        lvl->lightDepths = (int*)malloc(w * h * sizeof(int));
        lvl->listenerCount = 0;

        for (int x = 0; x < w; x++) {
            for (int y = 0; y < d; y++) {
                for (int z = 0; z < h; z++) {
                    int i = (y * lvl->height + z) * lvl->width + x;
                    lvl->blocks[i] = (y <= d * 2 / 3 ? 1 : 0);
                }
            }
        }

        Level_calcLightDepths(lvl, 0, 0, w, h);
        Level_load(lvl);
    }
    return lvl;
}

void Level_destroy(Level* self) {
    if (self) {
        free(self->blocks);
        free(self->lightDepths);
        free(self);
    }
}

void Level_load(Level* self) {
    if (!self) return;
    gzFile file = gzopen("level.dat", "rb");
    if (!file) {
        perror("Failed to open level.dat for reading");
        return;
    }

    int totalBytes = self->width * self->height * self->depth;
    int bytesRead = gzread(file, self->blocks, totalBytes);
    if (bytesRead != totalBytes) {
        fprintf(stderr, "Warning: Only read %d of %d bytes from level.dat\n", bytesRead, totalBytes);
    }
    gzclose(file);

    Level_calcLightDepths(self, 0, 0, self->width, self->height);

    for (int i = 0; i < self->listenerCount; i++) {
        if (self->listeners[i].allChanged) {
            self->listeners[i].allChanged(self->listeners[i].obj);
        }
    }
}

void Level_save(Level* self) {
    if (!self) return;
    gzFile file = gzopen("level.dat", "wb");
    if (!file) {
        perror("Failed to open level.dat for writing");
        return;
    }

    int totalBytes = self->width * self->height * self->depth;
    int bytesWritten = gzwrite(file, self->blocks, totalBytes);
    if (bytesWritten != totalBytes) {
        fprintf(stderr, "Warning: Only wrote %d of %d bytes to level.dat\n", bytesWritten, totalBytes);
    }
    gzclose(file);
}

void Level_calcLightDepths(Level* self, int x0, int y0, int x1, int y1) {
    if (!self) return;
    for (int x = x0; x < x0 + x1; x++) {
        for (int z = y0; z < y0 + y1; z++) {
            int oldDepth = self->lightDepths[x + z * self->width];
            int y = self->depth - 1;

            while (y > 0 && !Level_isLightBlocker(self, x, y, z)) {
                y--;
            }

            self->lightDepths[x + z * self->width] = y;
            if (oldDepth != y) {
                int yl0 = oldDepth < y ? oldDepth : y;
                int yl1 = oldDepth > y ? oldDepth : y;

                for (int i = 0; i < self->listenerCount; i++) {
                    if (self->listeners[i].lightColumnChanged) {
                        self->listeners[i].lightColumnChanged(self->listeners[i].obj, x, z, yl0, yl1);
                    }
                }
            }
        }
    }
}

void Level_addListener(Level* self, LevelListener listener) {
    if (!self) return;
    if (self->listenerCount < MAX_LISTENERS) {
        self->listeners[self->listenerCount++] = listener;
    }
}

void Level_removeListener(Level* self, LevelListener listener) {
    if (!self) return;
    for (int i = 0; i < self->listenerCount; i++) {
        if (self->listeners[i].obj == listener.obj) {
            for (int j = i; j < self->listenerCount - 1; j++) {
                self->listeners[j] = self->listeners[j + 1];
            }
            self->listenerCount--;
            break;
        }
    }
}

bool Level_isTile(Level* self, int x, int y, int z) {
    if (!self) return false;
    if (x >= 0 && y >= 0 && z >= 0 && x < self->width && y < self->depth && z < self->height) {
        return self->blocks[(y * self->height + z) * self->width + x] == 1;
    }
    return false;
}

bool Level_isSolidTile(Level* self, int x, int y, int z) {
    return Level_isTile(self, x, y, z);
}

bool Level_isLightBlocker(Level* self, int x, int y, int z) {
    return Level_isSolidTile(self, x, y, z);
}

int Level_getCubes(Level* self, AABB aabb, AABB* outCubes, int maxCubes) {
    if (!self || !outCubes) return 0;
    int count = 0;
    int x0 = (int)aabb.x0;
    int x1 = (int)(aabb.x1 + 1.0f);
    int y0 = (int)aabb.y0;
    int y1 = (int)(aabb.y1 + 1.0f);
    int z0 = (int)aabb.z0;
    int z1 = (int)(aabb.z1 + 1.0f);

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 > self->width) x1 = self->width;
    if (y1 > self->depth) y1 = self->depth;
    if (z1 > self->height) z1 = self->height;

    for (int x = x0; x < x1; x++) {
        for (int y = y0; y < y1; y++) {
            for (int z = z0; z < z1; z++) {
                if (Level_isSolidTile(self, x, y, z)) {
                    if (count < maxCubes) {
                        outCubes[count++] = AABB_create((float)x, (float)y, (float)z, (float)(x + 1), (float)(y + 1), (float)(z + 1));
                    } else {
                        return count;
                    }
                }
            }
        }
    }
    return count;
}

float Level_getBrightness(Level* self, int x, int y, int z) {
    if (!self) return 1.0f;
    float dark = 0.8f;
    float light = 1.0f;
    if (x < 0 || y < 0 || z < 0 || x >= self->width || y >= self->depth || z >= self->height) {
        return light;
    } else {
        return y < self->lightDepths[x + z * self->width] ? dark : light;
    }
}

void Level_setTile(Level* self, int x, int y, int z, int type) {
    if (!self) return;
    if (x >= 0 && y >= 0 && z >= 0 && x < self->width && y < self->depth && z < self->height) {
        self->blocks[(y * self->height + z) * self->width + x] = (unsigned char)type;
        Level_calcLightDepths(self, x, z, 1, 1);

        for (int i = 0; i < self->listenerCount; i++) {
            if (self->listeners[i].tileChanged) {
                self->listeners[i].tileChanged(self->listeners[i].obj, x, y, z);
            }
        }
    }
}
