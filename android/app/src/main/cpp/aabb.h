#ifndef AABB_H
#define AABB_H

#include <stdbool.h>

typedef struct {
    float epsilon;
    float x0;
    float y0;
    float z0;
    float x1;
    float y1;
    float z1;
} AABB;

AABB AABB_create(float x0, float y0, float z0, float x1, float y1, float z1);
AABB AABB_expand(AABB self, float xa, float ya, float za);
AABB AABB_grow(AABB self, float xa, float ya, float za);
float AABB_clipXCollide(AABB self, AABB c, float xa);
float AABB_clipYCollide(AABB self, AABB c, float ya);
float AABB_clipZCollide(AABB self, AABB c, float za);
bool AABB_intersects(AABB self, AABB c);
void AABB_move(AABB* self, float xa, float ya, float za);

#endif /* AABB_H */
