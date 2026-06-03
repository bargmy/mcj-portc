#include "player.h"
#include "common.h"
#include "input.h"

Player* Player_create(Level* level) {
    Player* p = (Player*)malloc(sizeof(Player));
    if (p) {
        Entity_init(&p->base, level);
        p->base.heightOffset = 1.62f;
    }
    return p;
}

void Player_destroy(Player* self) {
    if (self) {
        free(self);
    }
}

void Player_tick(Player* self, void* window) {
    if (!self) return;

    Entity_tick(&self->base);

    float xa = 0.0f;
    float ya = 0.0f;

    // Use GlobalInput which can be populated by keyboard or touch
    if (GlobalInput.forward) ya--;
    if (GlobalInput.back) ya++;
    if (GlobalInput.left) xa--;
    if (GlobalInput.right) xa++;
    
    // Joystick support
    if (GlobalInput.moveX != 0.0f) xa = GlobalInput.moveX;
    if (GlobalInput.moveY != 0.0f) ya = GlobalInput.moveY;

    // Jump
    if (GlobalInput.jump && self->base.onGround) {
        self->base.yd = 0.12f;
    }

    Entity_moveRelative(&self->base, xa, ya, self->base.onGround ? 0.02f : 0.005f);
    self->base.yd = (float)((double)self->base.yd - 0.005);
    Entity_move(&self->base, self->base.xd, self->base.yd, self->base.zd);

    self->base.xd *= 0.91f;
    self->base.yd *= 0.98f;
    self->base.zd *= 0.91f;

    if (self->base.onGround) {
        self->base.xd *= 0.8f;
        self->base.zd *= 0.8f;
    }
}
