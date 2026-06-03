#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "common.h"

typedef struct {
    Entity base;
} Player;

Player* Player_create(Level* level);
void Player_destroy(Player* self);
void Player_tick(Player* self, void* window);

#endif /* PLAYER_H */
