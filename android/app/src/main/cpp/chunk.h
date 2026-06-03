#ifndef CHUNK_H
#define CHUNK_H

#include "aabb.h"
#include "level.h"
#include "tesselator.h"

typedef struct Chunk {
    AABB aabb;
    Level* level;
    int x0, y0, z0;
    int x1, y1, z1;
    bool dirty;
#ifdef ANDROID_PORT
    unsigned int vboIds[2]; // Vertex Buffer IDs
    unsigned int tboIds[2]; // Texture Buffer IDs
    int vertexCounts[2];
#else
    unsigned int lists;
#endif
} Chunk;

extern int Chunk_rebuiltThisFrame;
extern int Chunk_updates;

Chunk* Chunk_create(Level* level, int x0, int y0, int z0, int x1, int y1, int z1);
void Chunk_destroy(Chunk* self);
void Chunk_render(Chunk* self, int layer);
void Chunk_setDirty(Chunk* self);

#endif /* CHUNK_H */
