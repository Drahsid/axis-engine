#ifndef __AXIS_GL_INCLUDED__
#define __AXIS_GL_INCLUDED__

#include <GL/glut.h>
#include "../include/include.h"

typedef struct {
    int width;
    int height;
} axis_viewport_t;

typedef struct {
    float l, r, b, t;
} axis_ortho_t;

typedef struct {
    vec3f_t pos;
    vec2f_t tex;
    rgba8_t col;
} axis_vtx_t;

static inline void InitializeOrthoWindow(char* windowName, axis_viewport_t vp, axis_ortho_t ortho, rgbaf_t bg)
{
    /* Form Setup */
    glutInitWindowSize(vp.width, vp.height);
    glutInitWindowPosition(0, 0); 

    /* Form Title */
    glutCreateWindow(windowName);

    /* Set Background Color */
    glClearColor(bg.r, bg.g, bg.b, bg.a);

    /* Main Projection Matrix */
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();

    /* Window Size */
    gluOrtho2D(ortho.l, ortho.r, ortho.b, ortho.t);

    /* Register Colors */
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

static inline void AxisGLEnd(void)
{
    glEnd();
    glFlush();
}

static inline void AxisGLSetFillColor(rgba8_t c)
{
    glColor4f(c.r / 255, c.g / 255, c.b / 255, c.a / 255);
}

static inline void AxisGL1Triangle(axis_vtx_t* vtx, int v0, int v1, int v2, int flag)
{
    int a, b, c;
    glBegin(GL_TRIANGLES);
    switch(flag)
    {
        case 0:
            a = v0; b = v1; c = v2;
            break;
        case 1:
            a = v1; b = v2; c = v0;
            break;
        case 2:
            a = v2; b = v0; c = v1;
            break;
        default:
            a = v0; b = v1; c = v2;
            break;
    }
    AxisGLSetFillColor(vtx[a].col); glVertex3f(vtx[a].pos.x, vtx[a].pos.y, vtx[a].pos.z);
    AxisGLSetFillColor(vtx[b].col); glVertex3f(vtx[b].pos.x, vtx[b].pos.y, vtx[b].pos.z);
    AxisGLSetFillColor(vtx[c].col); glVertex3f(vtx[c].pos.x, vtx[c].pos.y, vtx[c].pos.z);
    AxisGLEnd();
}

static inline void AxisGL2Triangles(axis_vtx_t* vtx, int v0, int v1, int v2, int flag0, int v3, int v4, int v5, int flag1)
{
    int a, b, c;
    glBegin(GL_TRIANGLES);
    switch(flag0)
    {
        case 0:
            a = v0; b = v1; c = v2;
            break;
        case 1:
            a = v1; b = v2; c = v0;
            break;
        case 2:
            a = v2; b = v0; c = v1;
            break;
        default:
            a = v0; b = v1; c = v2;
            break;
    }
    AxisGLSetFillColor(vtx[a].col); glVertex3f(vtx[a].pos.x, vtx[a].pos.y, vtx[a].pos.z);
    AxisGLSetFillColor(vtx[b].col); glVertex3f(vtx[b].pos.x, vtx[b].pos.y, vtx[b].pos.z);
    AxisGLSetFillColor(vtx[c].col); glVertex3f(vtx[c].pos.x, vtx[c].pos.y, vtx[c].pos.z);

    switch(flag1)
    {
        case 0:
            a = v3; b = v4; c = v5;
            break;
        case 1:
            a = v4; b = v5; c = v3;
            break;
        case 2:
            a = v5; b = v3; c = v4;
            break;
        default:
            a = v3; b = v4; c = v5;
            break;
    }
    AxisGLSetFillColor(vtx[a].col); glVertex3f(vtx[a].pos.x, vtx[a].pos.y, vtx[a].pos.z);
    AxisGLSetFillColor(vtx[b].col); glVertex3f(vtx[b].pos.x, vtx[b].pos.y, vtx[b].pos.z);
    AxisGLSetFillColor(vtx[c].col); glVertex3f(vtx[c].pos.x, vtx[c].pos.y, vtx[c].pos.z);
    AxisGLEnd();
}

static inline void AxisGLQuadrangle(axis_vtx_t* vtx, int v0, int v1, int v2, int v3, int flag)
{
    int a, b, c, d;
    glBegin(GL_QUADS);
    switch(flag)
    {
        case 0:
            a = v0; b = v1; c = v2, d = v3;
            break;
        case 1:
            a = v1; b = v2; c = v3, d = v0;
            break;
        case 2:
            a = v2; b = v3; c = v0, d = v1;
            break;
        case 3:
            a = v3; b = v0; c = v1; d = v2;
            break;
        default:
            a = v0; b = v1; c = v2, d = v3;
            break;
    }
    AxisGLSetFillColor(vtx[a].col); glVertex3f(vtx[a].pos.x, vtx[a].pos.y, vtx[a].pos.z);
    AxisGLSetFillColor(vtx[b].col); glVertex3f(vtx[b].pos.x, vtx[b].pos.y, vtx[b].pos.z);
    AxisGLSetFillColor(vtx[c].col); glVertex3f(vtx[c].pos.x, vtx[c].pos.y, vtx[c].pos.z);
    AxisGLSetFillColor(vtx[d].col); glVertex3f(vtx[d].pos.x, vtx[d].pos.y, vtx[d].pos.z);
    AxisGLEnd();
}

static inline void AxisGLFillRectangle(float ulx, float uly, float lrx, float lry)
{
    glRectf(-(ulx), uly, -(lrx), lry);
}

#endif /* __AXIS_GL_INCLUDED__ */