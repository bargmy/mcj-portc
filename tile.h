#ifndef TILE_H
#define TILE_H

#include "tesselator.h"

// Forward declaration of Level to avoid circular dependency
struct Level;

typedef struct {
    int tex;
} Tile;

// Declare static instances for rock and grass
extern Tile Tile_rock;
extern Tile Tile_grass;

void Tile_init(void);
void Tile_render(Tile* self, Tesselator* t, struct Level* level, int layer, int x, int y, int z);
void Tile_renderFace(Tile* self, Tesselator* t, int x, int y, int z, int face);

#endif /* TILE_H */
