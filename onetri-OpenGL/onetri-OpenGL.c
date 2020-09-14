#include <stdio.h>
#include "../video.h"
#include "../OpenGL/axis-gl.h"

typedef struct {
    float urx, ury;
    float llx, lly;
} axis_rectangle_t;

axis_viewport_t vp = (axis_viewport_t){
    SCREEN_WD, SCREEN_HT
};

axis_ortho_t ortho = (axis_ortho_t){
    -(SCREEN_WD / 2.0f)
    , (SCREEN_WD / 2.0f)
    , -(SCREEN_HT / 2.0f)
    , (SCREEN_HT / 2.0f)
};

axis_vtx_t tri[4] = {
    {-64.0f, 64.0f, 0.0f, 0.0f, 0.0f, 0, 255, 0, 255}
    , {64.0f, 64.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 255}
    , {64.0f, -64.0f, 0.0f, 0.0f, 0.0f, 0, 0, 255, 255}
    , {-64.0f, -64.0f, 0.0f, 0.0f, 0.0f, 255, 0, 0, 255}
};

void drawInnerRect(void)
{
    /* Blue Square */
    rgba8_t innerRect = (rgba8_t){64, 64, 255, 255};
    AxisGLSetFillColor(innerRect);
    axis_rectangle_t inner_rect = (axis_rectangle_t){
        ortho.r - 20, ortho.t - 20,
        -(ortho.r - 20), -(ortho.t - 20)
    };
    glRectf(inner_rect.urx, inner_rect.ury, inner_rect.llx, inner_rect.lly);
    glFlush();
}

void rotateTri(void)
{
    //glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    AxisGL2Triangles(tri, 0, 1, 2, 0, 0, 2, 3, 0);
    glRotatef(0.02f, 0, 0, 1);
    glFlush();
}

int main (int argc, char** argv) 
{ 
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
    
    /* Initialize Window */
    rgbaf_t bg_color = (rgbaf_t){0.25f, 0.25f, 0.25f, 1.0f};
    InitializeOrthoWindow("onetri-OpenGL", vp, ortho, bg_color);

    drawInnerRect();

    /* Draw and Rotate */
    glutIdleFunc(rotateTri);

    glutMainLoop();
}