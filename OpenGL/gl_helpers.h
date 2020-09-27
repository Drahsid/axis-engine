#ifndef __OPENGL_HELPERS_INCLUDED__
#define __OPENGL_HELPERS_INCLUDED__

#include <GL/glut.h>
#include <math.h>
#include "../include/include.h"

typedef enum pTypes_e {
    GL_PROJ_PERSPECTIVE
    , GL_PROJ_ORTHOGRAPHIC
} PROJ_TYPE;

typedef struct {
    int x, y;
    int width, height;
} glRectangle_t;

static inline mtxf4_t MakePersp(float fovy, float aspect, float near, float far)
{
    if (fovy <= 0.0f || fovy >= M_PI)
        fprintf(stderr, "fovy out of range!\n");
    
    if (near <= 0.0f)
        fprintf(stderr, "near out of range!\n");
    
    if (far <= 0.0f)
        fprintf(stderr, "far out of range!\n");
    
    if (near >= far)
        fprintf(stderr, "near out of range!\n");

    float yScale = 1.0f / tanf(fovy * 0.5f);
    float xScale = yScale / aspect;

    mtxf4_t result;

    result.mf[0][0] = xScale;
    result.mf[0][1] = result.mf[0][2] = result.mf[0][3] = 0.0f;

    result.mf[1][0] = yScale;
    result.mf[1][1] = result.mf[1][2] = result.mf[1][3] = 0.0f;

    result.mf[2][0] = result.mf[2][1] = 0.0f;
    result.mf[2][2] = far / (near - far);
    result.mf[2][3] = -1.0f;

    result.mf[3][0] = result.mf[3][1] = result.mf[3][3] = 0.0f;
    result.mf[3][2] = near * far / (near - far);

    return result;
}

static inline mtxf4_t MakeOrtho(float width, float height, float znear, float zfar)
{
    mtxf4_t result;

    result.mf[0][0] = 2.0f / width;
    result.mf[0][1] = result.mf[0][2] = result.mf[0][3] = 0.0f;

    result.mf[1][1] = 2.0f / height;
    result.mf[1][0] = result.mf[1][2] = result.mf[1][3] = 0.0f;

    result.mf[2][2] = 1.0f / (znear - zfar);
    result.mf[2][0] = result.mf[2][1] = result.mf[2][3] = 0.0f;

    result.mf[3][0] = result.mf[3][1] = 0.0f;
    result.mf[3][2] = znear / (znear - zfar);
    result.mf[3][3] = 1.0f;

    return result;
}

static inline mtxf4_t MakeOrthoOffCenter(float left, float right, float bottom, float top, float znear, float zfar)
{
    mtxf4_t result;

    result.mf[0][0] = 2.0f / (right - left);
    result.mf[0][1] = result.mf[0][2] = result.mf[0][3] = 0.0f;

    result.mf[1][1] = 2.0f / (top - bottom);
    result.mf[1][0] = result.mf[1][2] = result.mf[1][3] = 0.0f;

    result.mf[2][2] = 1.0f / (znear - zfar);
    result.mf[2][0] = result.mf[2][1] = result.mf[2][3] = 0.0f;

    result.mf[3][0] = (left + right) / (left - right);
    result.mf[3][1] = (top + bottom) / (bottom - top);
    result.mf[3][2] = znear / (znear - zfar);
    result.mf[3][3] = 1.0f;

    return result;
}

static inline void MakeViewport(PROJ_TYPE projType, glRectangle_t clientRect, float near, float far)
{
    if (clientRect.width <= 0 || clientRect.height <= 0)
        return;
    
    glViewport(0, 0, clientRect.width, clientRect.height);

    float aspect = clientRect.width / clientRect.height;
    //mtxf4_t projMtx = mtxf4_identity;

    switch(projType)
    {
        case GL_PROJ_PERSPECTIVE:
            //projMtx = MakePersp((M_PI / 3.0f), aspect, near, far);
            gluPerspective((M_PI / 3.0f), aspect, near, far);
            break;
        case GL_PROJ_ORTHOGRAPHIC:
            //projMtx = MakeOrthoOffCenter(clientRect.x, (clientRect.x + clientRect.width), (clientRect.y + clientRect.height), clientRect.y, near, far);
            gluOrtho2D(clientRect.x, (clientRect.x + clientRect.width), (clientRect.y + clientRect.height), clientRect.y);
            break;
    }

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glMultMatrixf((float*)projMtx.mf);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static inline void SetRenderDefaults(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClearColor(0.27f, 0.27f, 0.27f, 1.0f);
    glClearDepth(5.0f);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glLightf(GL_LIGHT0, GL_AMBIENT, 1.0f);
    glLightf(GL_LIGHT1, GL_DIFFUSE, 1.0f);
    glLightf(GL_LIGHT2, GL_SPECULAR, 1.0f);
    glLightf(GL_LIGHT3, GL_POSITION, 1.0f);
    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFlush();
}

#endif /* __OPENGL_HELPERS_INCLUDED__ */