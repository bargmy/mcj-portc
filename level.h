#ifndef LEVEL_H
#define LEVEL_H

#include "aabb.h"
#include <stdbool.h>

#define MAX_LISTENERS 8

typedef struct {
    void (*tileChanged)(void* obj, int x, int y, int z);
    void (*lightColumnChanged)(void* obj, int x, int z, int y0, int y1);
    void (*allChanged)(void* obj);
    void* obj;
} LevelListener;

typedef struct Level {
    int width;
    int height;
    int depth;
    unsigned char* blocks;
    int* lightDepths;
    LevelListener listeners[MAX_LISTENERS];
    int listenerCount;
} Level;

Level* Level_create(int w, int h, int d);
void Level_destroy(Level* self);
void Level_load(Level* self);
void Level_save(Level* self);
void Level_calcLightDepths(Level* self, int x0, int y0, int x1, int y1);
void Level_addListener(Level* self, LevelListener listener);
void Level_removeListener(Level* self, LevelListener listener);
bool Level_isTile(Level* self, int x, int y, int z);
bool Level_isSolidTile(Level* self, int x, int y, int z);
bool Level_isLightBlocker(Level* self, int x, int y, int z);
int Level_getCubes(Level* self, AABB aabb, AABB* outCubes, int maxCubes);
float Level_getBrightness(Level* self, int x, int y, int z);
void Level_setTile(Level* self, int x, int y, int z, int type);

#endif /* LEVEL_H */
