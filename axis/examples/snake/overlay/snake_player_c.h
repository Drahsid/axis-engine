#ifndef SNAKE_PLAYER_C
#define SNAKE_PLAYER_C

#include "snake_player.h"
#include "engine/heap.h"

#define RAND (pcg32_random())
#define RAND_POS_SCALED(resolution) ((-(((resolution) - 64.0f) / 2.0f)) + ((RAND % (((resolution) - 64) / 16)) * 16.0f))

void snake_player_reset_food(app_context_t* app, snake_player_t* snake) {
    int v, h;
    v = 1;
    while (v == 1) {
        v = 0;
        app->food.position = vec_new((float)RAND_POS_SCALED(SCREEN_WD), (float)RAND_POS_SCALED(SCREEN_HT), 0);
        for (h = 0; h < snake->body_count; h++) {
            if (vec3_equals(app->food.position, snake->body_list[h]->position)) {
                v = 1;
                break;
            }
        }
    }
}

void snake_player_construct(app_context_t* app, snake_player_t* snake) {
    int index;

    snake->actor.position = vec3f_zero;
    snake->actor.rotation = vec3f_zero;
    snake->actor.size = sizeof(snake_player_t);
    snake->actor.step = 0;
    snake->actor.draw = 0;
    snake->actor.type = ENTITY_TYPE_ACTOR;
    snake->direction = DIRECTION_RIGHT;
    snake->update_modulo = 8;
    snake->actor.deconstruct;

    snake_player_body_reset(app, snake);
}

void snake_player_deconstruct(app_context_t* app, snake_player_t* snake) {
    int index;

    if (snake->body_list) {
        for (index = 0; index < snake->body_count; index++) {
            free(snake->body_list[index]);
        }

        free(snake->body_list);
    }

    snake->body_list = 0;
    snake->body_count = 0;
}

void snake_player_body_push(app_context_t* app, snake_player_t* snake) {
    int index = snake->body_count;
    snake->body_count++;

    if (snake->body_list) {
        snake->body_list = (actor_t**)realloc(snake->body_list, sizeof(actor_t*) * snake->body_count);
    }
    else {
        snake->body_list = malloc(sizeof(actor_t*));
    }

    snake->body_list[index] = (actor_t*)malloc(sizeof(actor_t));
    snake->body_list[index]->rotation = vec3f_zero;
    snake->body_list[index]->size = sizeof(actor_t);
    snake->body_list[index]->step = 0;
    snake->body_list[index]->draw = 0;
    if (index) {
        if (snake->direction == DIRECTION_RIGHT) snake->body_list[index]->position = vec_new(snake->body_list[index - 1]->position.x - 1, snake->body_list[index - 1]->position.y, 0);
        if (snake->direction == DIRECTION_LEFT) snake->body_list[index]->position = vec_new(snake->body_list[index - 1]->position.x + 1, snake->body_list[index - 1]->position.y, 0);
        if (snake->direction == DIRECTION_UP) snake->body_list[index]->position = vec_new(snake->body_list[index - 1]->position.x, snake->body_list[index - 1]->position.y - 1, 0);
        if (snake->direction == DIRECTION_DOWN) snake->body_list[index]->position = vec_new(snake->body_list[index - 1]->position.x, snake->body_list[index - 1]->position.y + 1, 0);
    }
    else {
        snake->body_list[index]->position = vec3f_zero;
    }
}

void snake_player_body_reset(app_context_t* app, snake_player_t* snake) {
    int index;

    snake_player_deconstruct(app, snake);

    snake_player_body_push(app, snake);
    snake_player_body_push(app, snake);
    snake_player_body_push(app, snake);
    snake->body_list[0]->position = snake->actor.position;

    snake->actor.position = vec_new(RAND_POS_SCALED(SCREEN_WD), RAND_POS_SCALED(SCREEN_HT), 0);
    snake_player_reset_food(app, snake);
}

void snake_player_step(app_context_t* app, snake_player_t* snake) {
    vec3f_t add;
    int index;

    int8_t v = 0;
    int8_t h = 0;

    if (app->input_context.controller[0].buttons[BUTTON_INDEX_CRIGHT].state >= BUTTON_STATE_PRESSED) h -= 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_CLEFT].state >= BUTTON_STATE_PRESSED) h += 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_CUP].state >= BUTTON_STATE_PRESSED) v -= 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_CDOWN].state >= BUTTON_STATE_PRESSED) v += 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_DRIGHT].state >= BUTTON_STATE_PRESSED) h += 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_DLEFT].state >= BUTTON_STATE_PRESSED) h -= 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_DUP].state >= BUTTON_STATE_PRESSED) v -= 1;
    if (app->input_context.controller[0].buttons[BUTTON_INDEX_DDOWN].state >= BUTTON_STATE_PRESSED) v += 1;

    if (h > 0) snake->direction = DIRECTION_RIGHT;
    if (h < 0) snake->direction = DIRECTION_LEFT;
    if (v > 0) snake->direction = DIRECTION_UP;
    if (v < 0) snake->direction = DIRECTION_DOWN;

    if ((app->graphics_context.frame_count % snake->update_modulo) == 0) {
        for (index = snake->body_count - 1; index >= 0; index--) {
            if (index) {
                snake->body_list[index]->position = snake->body_list[index - 1]->position;
            }
            else {
                switch (snake->direction)
                {
                case DIRECTION_RIGHT:
                    add = vec_new(16, 0, 0);
                    break;
                case DIRECTION_UP:
                    add = vec_new(0, 16, 0);
                    break;
                case DIRECTION_LEFT:
                    add = vec_new(-16, 0, 0);
                    break;
                case DIRECTION_DOWN:
                    add = vec_new(0, -16, 0);
                    break;
                }

                vec3f_add_assignment(&snake->actor.position, &add);

                snake->body_list[index]->position = snake->actor.position;

                add = snake->actor.position;

                if (vec3_equals(app->food.position, add)) {
                    snake_player_body_push(app, snake);
                    snake_player_reset_food(app, snake);
                }
            }
        }

        for (index = 1; index < snake->body_count; index++) {
            if (snake->body_list[index]->position.x == add.x && snake->body_list[index]->position.y == add.y) {
                printf("Player got hecked!\n");
                snake_player_body_reset(app, snake);
            }
        }

        if (snake->actor.position.x > (SCREEN_WD / 2.0f) - (16 * 2) || snake->actor.position.x < -(SCREEN_WD / 2.0f) + (16 * 2)
        || snake->actor.position.y > (SCREEN_HT / 2.0f) - (16 * 2) || snake->actor.position.y < -(SCREEN_HT / 2.0f) + (16 * 2)) {
            snake_player_body_reset(app, snake);
        }
    }
}

#endif

