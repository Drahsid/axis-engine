/* Cross-Platform [H] Engine Resource Reviewer [Y] */
#include <stdio.h>
#include "../OpenGL/gl_helpers.h"
#include "../include/include.h"
#include "camera.h"

glRectangle_t vp = (glRectangle_t){
    0, 0, 640, 480
};

rgbaf_t bg_color = (rgbaf_t){
    0.27f, 0.27f, 0.27f, 1.0f
};

void DrawFloorGrid(void)
{
    glLineWidth(1.0f);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    for (int i = -8; i <= 8; i++)
    {
        if (i == 0)
            glColor3f(0.3, 0.3, 1.0f); // Z Axis
        else
            glColor3f(0.25, 0.25, 0.25);
        glVertex3s(i, 0, -8);
        glVertex3s(i, 0, 8);

        if (i == 0)
            glColor3f(1.0, 0.3, 0.3f); // X Axis
        else
            glColor3f(0.25, 0.25, 0.25);
        glVertex3s(-8, 0, i);
        glVertex3s(8, 0, i);
    }
    glEnd();
}

void DrawTriangle(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.0, 0.0f); glVertex2i(200, 125);
    glColor3f(0.0f, 1.0, 0.0f); glVertex2i(100, 375);
    glColor3f(0.0f, 0.0, 1.0f); glVertex2i(300, 375);
    glEnd();
    glFlush();
}

void CreateWindow(char* windowName, glRectangle_t vp)
{
    glutInitWindowSize(vp.width, vp.height);
    glutInitWindowPosition(vp.x, vp.y);
    glutCreateWindow(windowName);
    glViewport(vp.x, vp.y, vp.width, vp.height);
    glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    gluOrtho2D(0, vp.width, 0, vp.height);
}

int main(int argc, char** argv) 
{
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    CreateWindow("Cherry", vp);
    DrawTriangle();

    glutMainLoop();
}