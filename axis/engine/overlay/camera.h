#ifndef CAMERA_H
#define CAMERA_H

#include <PR/gbi.h>
#include "engine/video.h"
#include "engine/entity.h"
#include "math/vector.h"

typedef enum {
    CAMERA_TYPE_PERSPECITIVE = 0,
    CAMERA_TYPE_ORTHOGRAPHIC = 1
} camera_type_t;

typedef struct {
    actor_t actor;
    vec3f_t forward;
    vec3f_t right;
    vec3f_t up;
    float fovy;
    float aspect;
    float near;
    float far;
    float scale;
    camera_type_t camera_type;
    uint16_t persp_norm;
} camera_t;

void camera_construct(camera_t* camera) {
    camera->actor.position = vec_new(0, 0, 0);
    camera->actor.rotation = vec_new(0, HPI, 0);
    camera->actor.type = ENTITY_TYPE_ACTOR;
    camera->actor.step = 0;
    camera->actor.draw = 0;
    camera->actor.size = sizeof(camera_t);
    camera->forward = vec3f_forward;
    camera->right = vec3f_right;
    camera->up = vec3f_up;
    camera->fovy = 90.0f;
    camera->near = 0.01f;
    camera->far = G_MAXZ;
    camera->scale = 1.0f;
    camera->aspect = SCREEN_WD / SCREEN_HT;
}

#endif