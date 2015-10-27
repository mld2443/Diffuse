#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <list>

#include "bitmap.h"
#include "bezier.h"

#define WINDOW_SIZE 512

int mouseX = -1, mouseY = -1;
int diffuse_window, color_picker;
int selected_color;
bool mouseLeftDown = false, mouseRightDown = false, mouseMiddleDown = false;
bool drawImage = false, clicked = false;

list<point<float>> points;
list<point<float>*> new_points;
point<float> *selected, *coloring;

list<bezier> curves;

void display_1(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    if (!drawImage) {
        for (auto &p : points)
            p.draw(&p == selected);
    }
    
    for (auto &curve : curves)
        curve.draw(50);
    
    glFlush();
    
    if (drawImage) {
        // an example of how to read the frame buffer with the curves you just drew
        float *pixels = new float[WINDOW_SIZE * WINDOW_SIZE * 3];
        glReadPixels(0, 0, WINDOW_SIZE, WINDOW_SIZE, GL_RGB, GL_FLOAT, pixels);
        
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
        glDrawPixels(WINDOW_SIZE, WINDOW_SIZE, GL_RGB, GL_FLOAT, pixels);
        glFlush();
        delete[] pixels;
    }
    
    glutSwapBuffers();
}

void init(void) {
    /* select clearing color 	*/
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glDisable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH) - 1, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display_2(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glBegin(GL_QUADS); {
        glColor3ub(0,0,0);
        glVertex2i(10, 35);
        glVertex2i(10, 20);
        glColor3ub(255,0,0);
        glVertex2i(266, 20);
        glVertex2i(266, 35);
        
        glColor3ub(0,0,0);
        glVertex2i(10, 80);
        glVertex2i(10, 65);
        glColor3ub(0,255,0);
        glVertex2i(266, 65);
        glVertex2i(266, 80);
        
        glColor3ub(0,0,0);
        glVertex2i(10, 125);
        glVertex2i(10, 110);
        glColor3ub(0,0,255);
        glVertex2i(266, 110);
        glVertex2i(266, 125);
        
        glColor3ubv(coloring->lColor);
        glVertex2i(10, 245);
        glVertex2i(10, 145);
        glVertex2i(138, 145);
        glVertex2i(138, 245);
        
        glColor3ubv(coloring->rColor);
        glVertex2i(138, 245);
        glVertex2i(138, 145);
        glVertex2i(266, 145);
        glVertex2i(266, 245);
    } glEnd();
    
    glBegin(GL_TRIANGLES); {
        glColor3ub(255,255,255);
        for (int i = 0; i < 3; i++) {
            glVertex2i(coloring->lColor[i] + 6, i * 45 + 13);
            glVertex2i(coloring->lColor[i] + 14, i * 45 + 13);
            glVertex2i(coloring->lColor[i] + 10, i * 45 + 20);
            
            glVertex2i(coloring->rColor[i] + 10, i * 45 + 35);
            glVertex2i(coloring->rColor[i] + 14, i * 45 + 42);
            glVertex2i(coloring->rColor[i] + 6, i * 45 + 42);
        }
    } glEnd();
    
    glFlush();
    glutSwapBuffers();
}

void mouse_2(int button, int state, int x, int y) {
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            mouseLeftDown = state == GLUT_DOWN;
            if (mouseLeftDown) {
                selected_color = 0;
                if (y-13 < 8 && abs(x-coloring->lColor[0]) < 5)
                    selected_color = 1;
                else if (y-35 < 8 && abs(x-coloring->rColor[0]) < 5)
                    selected_color = 2;
                else if (y-58 < 8 && abs(x-coloring->lColor[1]) < 5)
                    selected_color = 3;
                else if (y-80 < 8 && abs(x-coloring->rColor[1]) < 5)
                    selected_color = 4;
                else if (y-103 < 8 && abs(x-coloring->lColor[2]) < 5)
                    selected_color = 5;
                else if (y-125 < 8 && abs(x-coloring->rColor[2]) < 5)
                    selected_color = 6;
            }
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

void reshape_2(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH) - 1, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void key_2(unsigned char c, int x, int y) {
    switch ( c )
    {
        case 13: //return
            glutDestroyWindow(color_picker);
            break;
            
        case 27: //escape
            glutDestroyWindow(color_picker);
            glutDestroyWindow(diffuse_window);
            exit(0);
            break;
    }
}

void motion_2(int x, int y) {
    mouseX = x;
    mouseY = y;
    
    if (selected_color) {
        switch (selected_color) {
            case 1:
                coloring->lColor[0] = (x < 265)? x-10 : 255;
                break;
            case 2:
                coloring->lColor[0] = (x < 265)? x-10 : 255;
                break;
            case 3:
                coloring->lColor[0] = (x < 265)? x-10 : 255;
                break;
            case 4:
                coloring->lColor[0] = (x < 265)? x-10 : 255;
                break;
            case 5:
                coloring->lColor[0] = (x < 265)? x-10 : 255;
                break;
            case 6:
                coloring->lColor[0] = (x < 265)? x-10 : 255;
                break;
        }
        glutPostRedisplay();
    }
}

void mouse_1(int button, int state, int x, int y) {
    selected = nullptr;
    if (state == GLUT_DOWN)
        for (auto &p : points)
            if (p.clicked(x, y))
                selected = &p;
    
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            mouseLeftDown = state == GLUT_DOWN;
            if (mouseLeftDown) {
                if (!selected) {
                    points.push_back(point<float>(x, y));
                    new_points.push_back(&points.back());
                }
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            mouseMiddleDown = state == GLUT_DOWN;
            break;
        case GLUT_RIGHT_BUTTON:
            mouseRightDown = state == GLUT_DOWN;
            if (selected) {
                coloring = selected;
                glutInitWindowSize(276, 276);
                glutInitWindowPosition(650, 200);
                color_picker = glutCreateWindow("Select Color");
                glutReshapeFunc (reshape_2);
                glutDisplayFunc(display_2);
                glutMouseFunc(mouse_2);
                glutMotionFunc(motion_2);
                glutKeyboardFunc(key_2);
            }
            break;
    }
    
    mouseX = x;
    mouseY = y;
    glutPostRedisplay();
}

void motion_1(int x, int y) {
    float dx, dy;
    dx = x - mouseX;
    dy = y - mouseY;
    mouseX = x;
    mouseY = y;
    
    if (selected) {
        selected->x = x;
        selected->y = y;
        glutPostRedisplay();
    }
}

void reshape_1(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH) - 1, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void key_1(unsigned char c, int x, int y) {
    switch ( c )
    {
        case 9: //tab
            break;
            
        case 13: //return
            if (new_points.size() > 1) {
                curves.push_back(bezier(new_points));
                new_points.clear();
                glutPostRedisplay();
            }
            break;
            
        case 8: //delete
            break;
            
        case 127: //backspace
            if (!drawImage){
                
            }
            break;
            
        case ' ':
            drawImage = !drawImage;
            glutPostRedisplay();
            break;
            
        case 27: //escape
            glutDestroyWindow(diffuse_window);
            exit(0);
            break;
            
        default:
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutInitWindowPosition(100, 100);
    diffuse_window = glutCreateWindow("CPSC 645 HW2 - Matthew Dillard");
    init();
    glutReshapeFunc (reshape_1);
    glutDisplayFunc(display_1);
    glutMouseFunc(mouse_1);
    glutMotionFunc(motion_1);
    glutKeyboardFunc(key_1);
    glutMainLoop();
    return 0;
}