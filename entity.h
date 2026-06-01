#ifndef ENTITY_H
#define ENTITY_H

#include "aabb.h"
#include "level.h"

typedef struct Entity {
    Level* level;
    float xo;
    float yo;
    float zo;
    float x;
    float y;
    float z;
    float xd;
    float yd;
    float zd;
    float yRot;
    float xRot;
    AABB bb;
    bool onGround;
    float heightOffset;
} Entity;

void Entity_init(Entity* self, Level* level);
void Entity_resetPos(Entity* self);
void Entity_setPos(Entity* self, float x, float y, float z);
void Entity_turn(Entity* self, float xo, float yo);
void Entity_tick(Entity* self);
void Entity_move(Entity* self, float xa, float ya, float za);
void Entity_moveRelative(Entity* self, float xa, float za, float speed);

#endif /* ENTITY_H */
