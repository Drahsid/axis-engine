#ifndef ENTITY_H
#define ENTITY_H

#include "math/vector.h"

typedef void (*step_func_t)(struct app_context_t*, struct entity_t*);

typedef enum {
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_ACTOR
} entitiy_type_t;

#define ENTITY_T_BODY entitiy_type_t type;\
    vec3f_t position;\
    vec3f_t rotation; // TODO: change to quat?

typedef struct
{
    ENTITY_T_BODY;
} entity_t;

typedef struct {
    union {
        entity_t entity;
        ENTITY_T_BODY;
    };
    step_func_t step;
    step_func_t draw;
    step_func_t construct;
    step_func_t deconstruct;
    uint64_t size;
    // actor overlay and init values should be pointed to
} actor_t;

#endif
