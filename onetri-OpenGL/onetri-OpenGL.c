#include <stdio.h>
#include "../OpenGL/axis-gl.h"

#define SCALE_FACTOR 2
#define SCREEN_WD 320 * SCALE_FACTOR
#define SCREEN_HT 240 * SCALE_FACTOR

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
    {-64.0f * SCALE_FACTOR, 64.0f * SCALE_FACTOR, 0.0f * SCALE_FACTOR, 0.0f, 0.0f, 0, 255, 0, 255}
    , {64.0f * SCALE_FACTOR, 64.0f * SCALE_FACTOR, 0.0f * SCALE_FACTOR, 0.0f, 0.0f, 0, 0, 0, 255}
    , {64.0f * SCALE_FACTOR, -64.0f * SCALE_FACTOR, 0.0f * SCALE_FACTOR, 0.0f, 0.0f, 0, 0, 255, 255}
    , {-64.0f * SCALE_FACTOR, -64.0f * SCALE_FACTOR, 0.0f * SCALE_FACTOR, 0.0f, 0.0f, 255, 0, 0, 255}
};

void displayFrame(void)
{
    static unsigned int delta = 0;
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    /* Inner Rectangle */
    glPushMatrix();
        glColor3f(0.25f, 0.25f, 1.0f);
        axis_rectangle_t inner_rect = (axis_rectangle_t){
            ortho.r - (20 * SCALE_FACTOR), ortho.t - (20 * SCALE_FACTOR),
            -(ortho.r - (20 * SCALE_FACTOR)), -(ortho.t - (20 * SCALE_FACTOR))
        };
        glRectf(inner_rect.urx, inner_rect.ury, inner_rect.llx, inner_rect.lly);
    glPopMatrix();

    /* Rotating Quad Object */
    glPushMatrix();
        glRotatef(0.02f * delta, 0, 0, 1);
        AxisGL2Triangles(tri, 0, 1, 2, 0, 0, 2, 3, 0);
        delta++;
    glPopMatrix();
    
    glFlush();
}

int main (int argc, char** argv) 
{ 
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
    
    /* Initialize Window */
    rgbaf_t bg_color = (rgbaf_t){0.25f, 0.25f, 0.25f, 1.0f};
    InitializeOrthoWindow("onetri-OpenGL", vp, ortho, bg_color);

    //drawInnerRect();

    /* Draw and Rotate */
    glutIdleFunc(displayFrame);

    glutMainLoop();
}