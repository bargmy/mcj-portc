#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

#include "level.h"
#include "chunk.h"
#include "player.h"
#include "hit_result.h"
#include "tesselator.h"

typedef struct {
    Level* level;
    Chunk** chunks;
    int xChunks;
    int yChunks;
    int zChunks;
    int chunkCount;
    Tesselator* t;
} LevelRenderer;

LevelRenderer* LevelRenderer_create(Level* level);
void LevelRenderer_destroy(LevelRenderer* self);
void LevelRenderer_render(LevelRenderer* self, Player* player, int layer);
void LevelRenderer_pick(LevelRenderer* self, Player* player);
void LevelRenderer_renderHit(LevelRenderer* self, HitResult h);
void LevelRenderer_setDirty(LevelRenderer* self, int x0, int y0, int z0, int x1, int y1, int z1);

#endif /* LEVEL_RENDERER_H */
