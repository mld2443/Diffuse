//
//  colorpicker.h
//  Diffuse
//
//  Created by Matthew Dillard on 10/27/15.
//

#ifndef colorpicker_h
#define colorpicker_h

#define BCOLOR_BUFFER 30

GLubyte valid[] = {255,255,255};
GLubyte invalid[] = {128,128,128};

int mouseX = -1, mouseY = -1;
int diffuse_window = 0, color_picker = 0;
bool mouseLeftDown = false, mouseRightDown = false;
bool drawImage = false;

point<float> *moving, *coloring;
int selected_color;

list<curve*> curves;
curve* selected;

void print(const int x, const int y, void* font, const string s){
    glRasterPos2f(x, y);
    int len, i;
    len = (int)s.length();
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, s[i]);
}

void display_big() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    GLubyte *degree, *elev, *param;
    switch (selected->get_type()) {
        case curve::lagrange:
            degree = invalid;
            elev = invalid;
            param = valid;
            break;
            
        case curve::bezier:
            degree = invalid;
            elev = valid;
            param = invalid;
            break;
            
        case curve::bspline:
            degree = valid;
            elev = invalid;
            param = invalid;
            break;
            
        case curve::catmullrom:
            degree = valid;
            elev = invalid;
            param = valid;
            break;
            
        default:
            degree = invalid;
            elev = invalid;
            param = invalid;
            break;
    }
    
    glColor3ubv(degree);
    print(10,18,GLUT_BITMAP_HELVETICA_10,"DEGREE:"+to_string(selected->get_degree()));
    glBegin(GL_LINES); {
        glVertex2i(74, 6);
        glVertex2i(74, 15);
        glVertex2i(70, 10);
        glVertex2i(79, 10);

        glVertex2i(70, 21);
        glVertex2i(79, 21);
    } glEnd();
    
    glColor3ubv(elev);
    print(110, 18, GLUT_BITMAP_HELVETICA_10, "ELEVATE DEGREE");
    
    glColor3ubv(param);
    print(2d30, 18, GLUadT_BITMAP_HELVETICA_10, "PARAM:"+to_string(selected->get_param()).substr(0,3));
    glBegin(GL_LINES); {
        glVertex2i(295, 6);
        glVertex2i(295, 15);
        glVertex2i(290, 10);
        glVertex2i(299, 10);
        
        glVertex2i(290, 21);
        glVertex2i(299, 21);
    } glEnd();
    
    glPushMatrix(); {
        glTranslatef(0, BCOLOR_BUFFER, 0);
        for (auto &p : selected->get_cpts()) {
            glBegin(GL_QUADS); {
                glColor3ub(0,0,0);
                glVertex2i(10, 20);
                glVertex2i(10, 10);
                glColor3ub(255,0,0);
                glVertex2i(266, 10);
                glVertex2i(266, 20);
                
                glColor3ub(0,0,0);
                glVertex2i(10, 45);
                glVertex2i(10, 35);
                glColor3ub(0,255,0);
                glVertex2i(266, 35);
                glVertex2i(266, 45);
                
                glColor3ub(0,0,0);
                glVertex2i(10, 70);
                glVertex2i(10, 60);
                glColor3ub(0,0,255);
                glVertex2i(266, 60);
                glVertex2i(266, 70);
                
                glColor3ub(p.lColor[0], p.lColor[1], p.lColor[2]);
                glVertex2i(276, 40);
                glVertex2i(276, 5);
                glVertex2i(311, 5);
                glVertex2i(311, 40);
                
                glColor3ub(p.rColor[0], p.rColor[1], p.rColor[2]);
                glVertex2i(276, 75);
                glVertex2i(276, 40);
                glVertex2i(311, 40);
                glVertex2i(311, 75);
            } glEnd();
            
            glBegin(GL_TRIANGLES); {
                glColor3ub(255,255,255);
                for (int i = 0; i < 3; i++) {
                    glVertex2i(p.lColor[i] + 6, i * 25 + 3);
                    glVertex2i(p.lColor[i] + 14, i * 25 + 3);
                    glVertex2i(p.lColor[i] + 10, i * 25 + 10);
                    
                    glVertex2i(p.rColor[i] + 10, i * 25 + 20);
                    glVertex2i(p.rColor[i] + 14, i * 25 + 27);
                    glVertex2i(p.rColor[i] + 6, i * 25 + 27);
                }
            } glEnd();
            glTranslatef(0, 85, 0);
        }
    } glPopMatrix();
    
    glFlush();
    glutSwapBuffers();
}

void mouse_big(int button, int state, int x, int y) {
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            mouseLeftDown = state == GLUT_DOWN;
            if (mouseLeftDown) {
                if (y < BCOLOR_BUFFER) {
                    bool degree, elev, param;
                    switch (selected->get_type()) {
                        case curve::lagrange:
                            degree = false;
                            elev = false;
                            param = true;
                            break;
                            
                        case curve::bezier:
                            degree = false;
                            elev = true;
                            param = false;
                            break;
                            
                        case curve::bspline:
                            degree = true;
                            elev = false;
                            param = false;
                            break;
                            
                        case curve::catmullrom:
                            degree = true;
                            elev = false;
                            param = true;
                            break;
                            
                        default:
                            degree = false;
                            elev = false;
                            param = false;
                            break;
                    }
                    
                    if (degree) {
                        if (x > 73 && x < 80) {
                            if (y > 5 && y < 16) {
                                selected->degree_inc();
                                glutPostRedisplay();
                                glutSetWindow(diffuse_window);
                                glutPostRedisplay();
                                glutSetWindow(color_picker);
                            }
                            else if (y > 16 && y < 26) {
                                selected->degree_dec();
                                glutPostRedisplay();
                                glutSetWindow(diffuse_window);
                                glutPostRedisplay();
                                glutSetWindow(color_picker);
                            }
                        }
                    }
                    if (elev) {
                        if (x > 109 && x < 202) {
                            if (y > 10 && y < 20) {
                                selected->elevate_degree();
                                glutReshapeWindow(321, glutGet(GLUT_WINDOW_HEIGHT) + 85);
                                glutSetWindow(diffuse_window);
                                glutPostRedisplay();
                                glutSetWindow(color_picker);
                            }
                        }
                    }
                    if (param) {
                        if (x > 289 && x < 300) {
                            if (y > 5 && y < 16) {
                                selected->param_inc();
                                glutPostRedisplay();
                                glutSetWindow(diffuse_window);
                                glutPostRedisplay();
                                glutSetWindow(color_picker);
                            }
                            else if (y > 16 && y < 26) {
                                selected->param_dec();
                                glutPostRedisplay();
                                glutSetWindow(diffuse_window);
                                glutPostRedisplay();
                                glutSetWindow(color_picker);
                            }
                        }
                    }
                }
                else {
                    selected_color = 0;
                    auto it = selected->get_cpts().begin();
                    for (int i = BCOLOR_BUFFER; i < glutGet(GLUT_WINDOW_HEIGHT) && selected_color == 0; i += 85, it++) {
                        if (y-i-3 > 0 && y-i-3 < 8 && abs((x-10)-it->lColor[0]) < 5)
                            selected_color = 1;
                        else if (y-i-20 > 0 && y-i-20 < 8 && abs((x-10)-it->rColor[0]) < 5)
                            selected_color = 2;
                        else if (y-i-28 > 0 && y-i-28 < 8 && abs((x-10)-it->lColor[1]) < 5)
                            selected_color = 3;
                        else if (y-i-45 > 0 && y-i-45 < 8 && abs((x-10)-it->rColor[1]) < 5)
                            selected_color = 4;
                        else if (y-i-53 > 0 && y-i-53 < 8 && abs((x-10)-it->lColor[2]) < 5)
                            selected_color = 5;
                        else if (y-i-70 > 0 && y-i-70 < 8 && abs((x-10)-it->rColor[2]) < 5)
                            selected_color = 6;
                    }
                    if (selected_color)
                        coloring = &*(--it);
                }
            }
            break;
        case GLUT_RIGHT_BUTTON:
            mouseRightDown = state == GLUT_DOWN;
            break;
    }
    
    mouseX = x;
    mouseY = y;
}

void display_small() {
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
        glVertex2i(10, 266);
        glVertex2i(10, 145);
        glVertex2i(138, 145);
        glVertex2i(138, 266);
        
        glColor3ub(coloring->rColor[0], coloring->rColor[1], coloring->rColor[2]);
        glVertex2i(138, 266);
        glVertex2i(138, 145);
        glVertex2i(266, 145);
        glVertex2i(266, 266);
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

void mouse_small(int button, int state, int x, int y) {
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

void reshape_cp(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH) - 1, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void key(unsigned char c, int x, int y);

void key_cp(unsigned char c, int x, int y) {
    switch(c)
    {
        case 13: //return
            coloring = nullptr;
            selected = nullptr;
            glutDestroyWindow(color_picker);
            color_picker = 0;
            glutSetWindow(diffuse_window);
            glutPostRedisplay();
            break;
            
        case 8:
            key(c, x, y);
            break;
            
        case 27: //escape
            glutDestroyWindow(color_picker);
            glutDestroyWindow(diffuse_window);
            exit(0);
            break;
    }
}

void motion_cp(int x, int y) {
    mouseX = x;
    mouseY = y;
    
    if (selected_color && mouseLeftDown) {
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
