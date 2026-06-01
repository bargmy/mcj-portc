#ifndef TESSELATOR_H
#define TESSELATOR_H

#include <stdbool.h>

#define MAX_VERTICES 100000

typedef struct {
    float* vertexBuffer;
    float* texCoordBuffer;
    float* colorBuffer;
    int vertices;
    float u;
    float v;
    float r;
    float g;
    float b;
    bool hasColor;
    bool hasTexture;
} Tesselator;

Tesselator* Tesselator_create(void);
void Tesselator_destroy(Tesselator* self);
void Tesselator_flush(Tesselator* self);
void Tesselator_init(Tesselator* self);
void Tesselator_tex(Tesselator* self, float u, float v);
void Tesselator_color(Tesselator* self, float r, float g, float b);
void Tesselator_vertex(Tesselator* self, float x, float y, float z);

#endif /* TESSELATOR_H */
