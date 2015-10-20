#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <math.h>
#include <assert.h>
#include "point.h"
#include "bitmap.h"

#define WINDOW_SIZE 512

int mouseX = -1, mouseY = -1;
bool mouseLeftDown = false, mouseRightDown = false, mouseMiddleDown = false;
bool drawImage = false;

using namespace std;

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // draw curves
    glFlush ( );
    
    if ( drawImage )
    {
        // an example of how to read the frame buffer with the curves you just drew
        float *pixels = new float [ WINDOW_SIZE * WINDOW_SIZE * 3 ];
        glReadPixels ( 0, 0, WINDOW_SIZE, WINDOW_SIZE, GL_RGB, GL_FLOAT, pixels );
        
        // an example of how to write an image to a file for debugging purposes
        //char name [ 256 ];
        //sprintf ( name, "screen.bmp" );
        //saveBMP ( name, pixels, WINDOW_SIZE, WINDOW_SIZE );
        
        // DOWNSAMPLE
        // for all pixels that are black in the lower resolution image, create a downsampled image averaging the pixels from the higher resolution image that are not black
        
        // AVERAGE
        // for each level starting at the bottom
        //		for each pixel that is not constrained, overwrite its color with the color from the previous level
        //		for some fixed number of iterations, (I used 100)
        //			replace each unconstrained pixel with the average of its neighbors
        
        // draw the final, high resolution image to the screen
        glDrawPixels ( WINDOW_SIZE, WINDOW_SIZE, GL_RGB, GL_FLOAT, pixels );
        glFlush ( );
        delete[] pixels;
    }
    
    glutSwapBuffers();
}

void init(void)
{
    /* select clearing color 	*/
    glClearColor ( 0.0, 0.0, 0.0, 0.0 );
    
    glDisable ( GL_DEPTH_TEST );
    
    glShadeModel ( GL_SMOOTH );
    
    glDisable ( GL_CULL_FACE );
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity ( );
    gluOrtho2D ( 0, glutGet ( GLUT_WINDOW_WIDTH ) - 1, glutGet ( GLUT_WINDOW_HEIGHT ) - 1, 0 );
    
    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity ( );
}

void mouse(int button, int state, int x, int y)
{
    switch ( button )
    {
        case GLUT_LEFT_BUTTON:
            mouseLeftDown = state == GLUT_DOWN;
            break;
        case GLUT_MIDDLE_BUTTON:
            mouseMiddleDown = state == GLUT_DOWN;
            break;
        case GLUT_RIGHT_BUTTON:
            mouseRightDown = state == GLUT_DOWN;
            break;
    }
    mouseX = x;
    mouseY = y;
}

void motion(int x, int y)
{
    float dx, dy;
    dx = ( x - mouseX );
    dy = ( y - mouseY );
    mouseX = x;
    mouseY = y;
    
    glutPostRedisplay ( );
}

void reshape ( int w, int h )
{
    glViewport ( 0, 0, w, h );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity ( );
    gluOrtho2D ( 0, glutGet ( GLUT_WINDOW_WIDTH ) - 1, glutGet ( GLUT_WINDOW_HEIGHT ) - 1, 0 );
    glMatrixMode ( GL_MODELVIEW );
}

void key ( unsigned char c, int x, int y )
{
    switch ( c )
    {
        case 'd':
        case 'D':
            drawImage = !drawImage;
            glutPostRedisplay ( );
            break;
        default:
            break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("CPSC 645 HW2 - Matthew Dillard");
    init();
    glutReshapeFunc (reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(key);
    glutMainLoop();
    return 0;
}