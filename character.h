#ifndef CHARACTER_H
#define CHARACTER_H

#include "vec3.h"
#include "entity.h"

typedef struct {
    Vec3 pos;
    float u;
    float v;
} Vertex;

Vertex Vertex_create(float x, float y, float z, float u, float v);
Vertex Vertex_remap(Vertex self, float u, float v);

typedef struct {
    Vertex vertices[4];
    int vertexCount;
} ModelPolygon;

ModelPolygon ModelPolygon_create(Vertex* vertices);
ModelPolygon ModelPolygon_createTex(Vertex* vertices, int u0, int v0, int u1, int v1);
void ModelPolygon_render(ModelPolygon* self);

typedef struct {
    Vertex vertices[8];
    ModelPolygon polygons[6];
    int xTexOffs;
    int yTexOffs;
    float x;
    float y;
    float z;
    float xRot;
    float yRot;
    float zRot;
} Cube;

Cube Cube_create(int xTexOffs, int yTexOffs);
void Cube_setTexOffs(Cube* self, int xTexOffs, int yTexOffs);
void Cube_addBox(Cube* self, float x0, float y0, float z0, int w, int h, int d);
void Cube_setPos(Cube* self, float x, float y, float z);
void Cube_render(Cube* self);

typedef struct {
    Entity base;
    Cube head;
    Cube body;
    Cube arm0;
    Cube arm1;
    Cube leg0;
    Cube leg1;
    float rot;
    float timeOffs;
    float speed;
    float rotA;
} Zombie;

Zombie* Zombie_create(Level* level, float x, float y, float z);
void Zombie_destroy(Zombie* self);
void Zombie_tick(Zombie* self);
void Zombie_render(Zombie* self, float a);

#endif /* CHARACTER_H */
