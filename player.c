#include "player.h"
#include "common.h"

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

void Player_tick(Player* self, GLFWwindow* window) {
    if (!self) return;

    Entity_tick(&self->base);

    float xa = 0.0f;
    float ya = 0.0f;

    // R key to reset position
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        Entity_resetPos(&self->base);
    }

    // Forward / Backward
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        ya--;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        ya++;
    }

    // Left / Right
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        xa--;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        xa++;
    }

    // Jump (Space)
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && self->base.onGround) {
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
