#ifndef SNAKE_PLAYER_H
#define SNAKE_PLAYER_H

#include "math/vector.h"
#include "engine/entity.h"
#include "examples/snake/app_context.h"
#include "math/pcg/pcg_basic.h"

typedef enum {
    DIRECTION_RIGHT = 0,
    DIRECTION_UP,
    DIRECTION_LEFT,
    DIRECTION_DOWN
} snake_direction_t;

typedef struct {
    actor_t actor;
    snake_direction_t direction;
    int update_modulo;
    uint32_t body_count;
    actor_t** body_list;
} snake_player_t;

#endif

