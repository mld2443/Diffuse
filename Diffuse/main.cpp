#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <list>

#include "bitmap.h"
#include "lagrange.h"
#include "bezier.h"
#include "bspline.h"
#include "catmullrom.h"
#include "colorpicker.h"

#define WINDOW_SIZE 512

list<point<float>> points;
list<point<float>*> new_points;

void display_1(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    for (auto &curve : curves)
        curve->draw(!drawImage);
    
    if (!drawImage) {
        for (auto &p : new_points)
            p->draw(p == coloring);
    }
    
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

void mouse_1(int button, int state, int x, int y) {
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            mouseLeftDown = state == GLUT_DOWN;
            
            if (!drawImage){
                if (mouseLeftDown) {
                    moving = nullptr;
                    for (auto &p : points)
                        if (p.clicked(x, y))
                            moving = &p;
                    
                    if (!moving) {
                        points.push_back(point<float>(x, y));
                        new_points.push_back(&points.back());
                    }
                }
                else {
                    moving = nullptr;
                }
                glutPostRedisplay();
            }
            break;
        case GLUT_RIGHT_BUTTON:
            mouseRightDown = state == GLUT_DOWN;
            
            if (!drawImage){
                if (mouseRightDown) {
                    moving = nullptr;
                    for (auto &p : points)
                        if (p.clicked(x, y))
                            moving = &p;
                    
                    if (moving) {
                        coloring = moving;
                        moving = nullptr;
                        
                        if (color_picker) {
                            glutDestroyWindow(color_picker);
                            color_picker = 0;
                        }
                        
                        glutInitWindowSize(276, 276);
                        glutInitWindowPosition(650, 200);
                        color_picker = glutCreateWindow("Select Color");
                        glutReshapeFunc (reshape_s);
                        glutDisplayFunc(display_s);
                        glutMouseFunc(mouse_s);
                        glutMotionFunc(motion_s);
                        glutKeyboardFunc(key_s);
                    }
                    glutSetWindow(diffuse_window);
                    glutPostRedisplay();
                }
            }
            break;
    }
    
    mouseX = x;
    mouseY = y;
}

void motion_1(int x, int y) {
    float dx, dy;
    dx = x - mouseX;
    dy = y - mouseY;
    mouseX = x;
    mouseY = y;
    
    if (moving) {
        moving->x = x;
        moving->y = y;
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

/*void save_file() {
    string filename;
    printf("save file: ");
    cin >> filename;
    ofstream file;
    file.open(filename);
    file << curves.size() << endl;
    for (auto &crv : curves) {
        //file << crv->get_type() << endl;
        file << crv->get_degree() << ' ' << crv->get_fidelity() << ' ' \
        << crv->get_param() << ' ' << crv->get_c_points().size() << endl;
        for (auto & pt : crv->get_c_points())
            file << pt.x << ' ' << pt.y << endl;
    }
    file.close();
}

void load_file() {
    string filename;
    printf("load file: ");
    cin >> filename;
    ifstream file;
    file.open(filename);
    curves.clear();
    unsigned int num_curves;
    file >> num_curves;
    for (unsigned int i = 0; i < num_curves; i++) {
        int type;
        file >> type;
        
        unsigned int degree, size;
        float fidel, param;
        file >> degree >> fidel >> param >> size;
        
        curve *crv;
        switch (type) {
            case curve::lagrange:
                crv = new lagrange(degree, fidel, param);
                break;
            case curve::bezier:
                crv = new bezier(degree, fidel, param);
                break;
            case curve::bspline:
                crv = new bspline(degree, fidel, param);
                break;
            case curve::catmullrom:
                crv = new catmullrom(degree, fidel, param);
                break;
                
            default:
                printf("file error: invalid type\n");
                break;
        }
        
        vector<Point> c_points;
        for (int j = 0; j < size; j++) {
            float x,y;
            file >> x >> y;
            c_points.push_back(Point(x, y));
        }
        crv->generate(c_points);
        curves.push_back(crv);
    }
    file.close();
}*/

void key_1(unsigned char c, int x, int y) {
    switch ( c )
    {
        case 9: //tab
            break;
            
        case 13: //return
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
            
        case '1':
            if (new_points.size() > 1) {
                curves.push_back(new lagrange(new_points));
                new_points.clear();
                glutPostRedisplay();
            }
            break;
            
        case '2':
            if (new_points.size() > 1) {
                curves.push_back(new bezier(new_points));
                new_points.clear();
                glutPostRedisplay();
            }
            break;
            
        case '3':
            if (new_points.size() > 1) {
                curves.push_back(new bspline(new_points));
                new_points.clear();
                glutPostRedisplay();
            }
            break;
            
        case '4':
            if (new_points.size() > 1) {
                curves.push_back(new catmullrom(new_points));
                new_points.clear();
                glutPostRedisplay();
            }
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
