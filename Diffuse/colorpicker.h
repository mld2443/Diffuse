//
//  colorpicker.h
//  Diffuse
//
//  Created by Matthew Dillard on 10/27/15.
//  Copyright © 2015 Matthew Dillard. All rights reserved.
//

#ifndef colorpicker_h
#define colorpicker_h

int mouseX = -1, mouseY = -1;
int diffuse_window = 0, color_picker = 0;
bool mouseLeftDown = false, mouseRightDown = false;
bool drawImage = false, clicked = false;

point<float> *moving, *coloring;
int selected_color;

list<curve*> curves;
curve* selected;

void display_s(void) {
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
        
        glColor3ub(coloring->lColor[0], coloring->lColor[1], coloring->lColor[2]);
        glVertex2i(10, 245);
        glVertex2i(10, 145);
        glVertex2i(138, 145);
        glVertex2i(138, 245);
        
        glColor3ub(coloring->rColor[0], coloring->rColor[1], coloring->rColor[2]);
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

void mouse_s(int button, int state, int x, int y) {
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            mouseLeftDown = state == GLUT_DOWN;
            if (mouseLeftDown) {
                selected_color = 0;
                if (y-13 > 0 && y-13 < 8 && abs((x-10)-coloring->lColor[0]) < 5)
                    selected_color = 1;
                else if (y-35 > 0 && y-35 < 8 && abs((x-10)-coloring->rColor[0]) < 5)
                    selected_color = 2;
                else if (y-58 > 0 && y-58 < 8 && abs((x-10)-coloring->lColor[1]) < 5)
                    selected_color = 3;
                else if (y-80 > 0 && y-80 < 8 && abs((x-10)-coloring->rColor[1]) < 5)
                    selected_color = 4;
                else if (y-103 > 0 && y-103 < 8 && abs((x-10)-coloring->lColor[2]) < 5)
                    selected_color = 5;
                else if (y-125 > 0 && y-125 < 8 && abs((x-10)-coloring->rColor[2]) < 5)
                    selected_color = 6;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            mouseRightDown = state == GLUT_DOWN;
            break;
    }
    
    mouseX = x;
    mouseY = y;
}

void reshape_s(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH) - 1, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void key_s(unsigned char c, int x, int y) {
    switch ( c )
    {
        case 13: //return
            coloring = nullptr;
            glutDestroyWindow(color_picker);
            color_picker = 0;
            glutSetWindow(diffuse_window);
            glutPostRedisplay();
            break;
            
        case 27: //escape
            glutDestroyWindow(color_picker);
            glutDestroyWindow(diffuse_window);
            exit(0);
            break;
    }
}

void motion_s(int x, int y) {
    mouseX = x;
    mouseY = y;
    
    if (selected_color) {
        switch (selected_color) {
            case 1:
                coloring->lColor[0] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 2:
                coloring->rColor[0] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 3:
                coloring->lColor[1] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 4:
                coloring->rColor[1] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 5:
                coloring->lColor[2] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 6:
                coloring->rColor[2] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
        }
        
        glutPostRedisplay();
        glutSetWindow(diffuse_window);
        glutPostRedisplay();
        glutSetWindow(color_picker);
    }
}

void display_b(void) {
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
        
        glColor3ub(coloring->lColor[0], coloring->lColor[1], coloring->lColor[2]);
        glVertex2i(10, 245);
        glVertex2i(10, 145);
        glVertex2i(138, 145);
        glVertex2i(138, 245);
        
        glColor3ub(coloring->rColor[0], coloring->rColor[1], coloring->rColor[2]);
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

void mouse_b(int button, int state, int x, int y) {
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            mouseLeftDown = state == GLUT_DOWN;
            if (mouseLeftDown) {
                selected_color = 0;
                if (y-13 > 0 && y-13 < 8 && abs((x-10)-coloring->lColor[0]) < 5)
                    selected_color = 1;
                else if (y-35 > 0 && y-35 < 8 && abs((x-10)-coloring->rColor[0]) < 5)
                    selected_color = 2;
                else if (y-58 > 0 && y-58 < 8 && abs((x-10)-coloring->lColor[1]) < 5)
                    selected_color = 3;
                else if (y-80 > 0 && y-80 < 8 && abs((x-10)-coloring->rColor[1]) < 5)
                    selected_color = 4;
                else if (y-103 > 0 && y-103 < 8 && abs((x-10)-coloring->lColor[2]) < 5)
                    selected_color = 5;
                else if (y-125 > 0 && y-125 < 8 && abs((x-10)-coloring->rColor[2]) < 5)
                    selected_color = 6;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            mouseRightDown = state == GLUT_DOWN;
            break;
    }
    
    mouseX = x;
    mouseY = y;
}

void reshape_b(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH) - 1, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void key_b(unsigned char c, int x, int y) {
    switch ( c )
    {
        case 13: //return
            coloring = nullptr;
            glutDestroyWindow(color_picker);
            color_picker = 0;
            glutSetWindow(diffuse_window);
            glutPostRedisplay();
            break;
            
        case 27: //escape
            glutDestroyWindow(color_picker);
            glutDestroyWindow(diffuse_window);
            exit(0);
            break;
    }
}

void motion_b(int x, int y) {
    mouseX = x;
    mouseY = y;
    
    if (selected_color) {
        switch (selected_color) {
            case 1:
                coloring->lColor[0] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 2:
                coloring->rColor[0] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 3:
                coloring->lColor[1] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 4:
                coloring->rColor[1] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 5:
                coloring->lColor[2] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
            case 6:
                coloring->rColor[2] = (x < 265)? ((x > 10)? x-10 : 0) : 255;
                break;
        }
        
        glutPostRedisplay();
        glutSetWindow(diffuse_window);
        glutPostRedisplay();
        glutSetWindow(color_picker);
    }
}

#endif /* colorpicker_h */
