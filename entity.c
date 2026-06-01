#include "entity.h"
#include "common.h"

void Entity_init(Entity* self, Level* level) {
    if (!self) return;
    self->level = level;
    self->xo = 0.0f;
    self->yo = 0.0f;
    self->zo = 0.0f;
    self->x = 0.0f;
    self->y = 0.0f;
    self->z = 0.0f;
    self->xd = 0.0f;
    self->yd = 0.0f;
    self->zd = 0.0f;
    self->yRot = 0.0f;
    self->xRot = 0.0f;
    self->bb = AABB_create(0, 0, 0, 0, 0, 0);
    self->onGround = false;
    self->heightOffset = 0.0f;
    Entity_resetPos(self);
}

void Entity_resetPos(Entity* self) {
    if (!self || !self->level) return;
    float rx = ((float)rand() / (float)RAND_MAX) * (float)self->level->width;
    float ry = (float)(self->level->depth + 10);
    float rz = ((float)rand() / (float)RAND_MAX) * (float)self->level->height;
    Entity_setPos(self, rx, ry, rz);
}

void Entity_setPos(Entity* self, float x, float y, float z) {
    if (!self) return;
    self->x = x;
    self->y = y;
    self->z = z;
    float w = 0.3f;
    float h = 0.9f;
    self->bb = AABB_create(x - w, y - h, z - w, x + w, y + h, z + w);
}

void Entity_turn(Entity* self, float xo, float yo) {
    if (!self) return;
    self->yRot = (float)((double)self->yRot + (double)xo * 0.15);
    self->xRot = (float)((double)self->xRot - (double)yo * 0.15);
    if (self->xRot < -90.0f) {
        self->xRot = -90.0f;
    }
    if (self->xRot > 90.0f) {
        self->xRot = 90.0f;
    }
}

void Entity_tick(Entity* self) {
    if (!self) return;
    self->xo = self->x;
    self->yo = self->y;
    self->zo = self->z;
}

void Entity_move(Entity* self, float xa, float ya, float za) {
    if (!self || !self->level) return;
    float xaOrg = xa;
    float yaOrg = ya;
    float zaOrg = za;

    AABB expanded = AABB_expand(self->bb, xa, ya, za);
    AABB aABBs[1024];
    int count = Level_getCubes(self->level, expanded, aABBs, 1024);

    for (int i = 0; i < count; i++) {
        ya = AABB_clipYCollide(aABBs[i], self->bb, ya);
    }
    AABB_move(&self->bb, 0.0f, ya, 0.0f);

    for (int i = 0; i < count; i++) {
        xa = AABB_clipXCollide(aABBs[i], self->bb, xa);
    }
    AABB_move(&self->bb, xa, 0.0f, 0.0f);

    for (int i = 0; i < count; i++) {
        za = AABB_clipZCollide(aABBs[i], self->bb, za);
    }
    AABB_move(&self->bb, 0.0f, 0.0f, za);

    self->onGround = (yaOrg != ya) && (yaOrg < 0.0f);

    if (xaOrg != xa) {
        self->xd = 0.0f;
    }
    if (yaOrg != ya) {
        self->yd = 0.0f;
    }
    if (zaOrg != za) {
        self->zd = 0.0f;
    }

    self->x = (self->bb.x0 + self->bb.x1) / 2.0f;
    self->y = self->bb.y0 + self->heightOffset;
    self->z = (self->bb.z0 + self->bb.z1) / 2.0f;
}

void Entity_moveRelative(Entity* self, float xa, float za, float speed) {
    if (!self) return;
    float dist = xa * xa + za * za;
    if (!(dist < 0.01f)) {
        dist = speed / sqrtf(dist);
        xa *= dist;
        za *= dist;
        float sinR = sinf((float)((double)self->yRot * M_PI / 180.0));
        float cosR = cosf((float)((double)self->yRot * M_PI / 180.0));
        self->xd += xa * cosR - za * sinR;
        self->zd += za * cosR + xa * sinR;
    }
}
