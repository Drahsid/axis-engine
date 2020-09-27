#ifndef __CAMERA_INCLUDED__
#define __CAMERA_INCLUDED__

#include <GL/glut.h>
#include <math.h>
#include "../include/include.h"

vec3f_t pos, rot;

#define CameraReset Reset
static inline void Reset(void)
{
    pos = (vec3f_t){0.0f, 0.0f, -15.0f};
    rot = (vec3f_t){0.0f, 0.0f, 0.0f};
}

static inline void CameraPosition(void)
{
    glRotatef(rot.x, 1.0f, 0.0, 0.0f);
    glRotatef(rot.y, 0.0f, 1.0f, 1.0f);
    glRotatef(rot.z, 0.0f, 0.0f, 1.0f);
    glTranslatef(pos.x, pos.y, pos.z);
}

static inline void CameraInitialize(void)
{
    CameraReset();
    CameraPosition();
}



#endif /* __CAMERA_INCLUDED__ */