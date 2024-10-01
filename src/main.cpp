#include <GL/freeglut.h>
#include <GL/gl.h>
#include <fstream>
#include <list>

#include "lagrange.h"
#include "bezier.h"
#include "bspline.h"
#include "catmullrom.h"
#include "colorpicker.h"
#include "diffuse.h"

#define WINDOW_SIZE 512
#define WINDOW_OFFX 100
#define WINDOW_OFFY 100

int smoothness = 25, iter = 512;
list<point<float>> points;

void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for (auto &curve : curves)
        curve->draw(!drawImage, curve == selected, coloring);

    if (!drawImage) {
        for (auto &p : points)
            p.draw((&p == moving) + (&p == coloring) * 2);
    }

    glFlush();

    if (drawImage) {
        GLubyte *pixels = new GLubyte[WINDOW_SIZE * WINDOW_SIZE * 3];
        glReadPixels(0, 0, WINDOW_SIZE, WINDOW_SIZE, GL_RGB, GL_UNSIGNED_BYTE, pixels);

        pixels = diffuse(WINDOW_SIZE, pixels, smoothness, iter);

        glDrawPixels(WINDOW_SIZE, WINDOW_SIZE, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glFlush();
        delete[] pixels;
    }

    glutSwapBuffers();
}

void init() {
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

void close_color_window(const bool condition) {
    if (condition) {
        glutDestroyWindow(color_picker);
        color_picker = 0;
        coloring = nullptr;
    }
}

void mouse(int button, int state, int x, int y) {
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
                    if (!moving)
                        for (auto crv : curves)
                            for (auto &p : crv->get_cpts())
                                if (p.clicked(x, y))
                                    moving = &p;

                    if (!moving) {
                        points.push_back(point<float>(x, y));
                    }
                }
                glutPostRedisplay();
            }
            break;
        case GLUT_RIGHT_BUTTON:
            mouseRightDown = state == GLUT_DOWN;

            if (!drawImage){
                if (mouseRightDown) {
                    coloring = nullptr;
                    selected = nullptr;
                    if (color_picker) {
                        glutDestroyWindow(color_picker);
                        color_picker = 0;
                    }

                    for (auto &p : points)
                        if (p.clicked(x, y))
                            coloring = &p;

                    if (coloring) {
                        glutInitWindowSize(276, 276);
                        glutInitWindowPosition(WINDOW_OFFX + WINDOW_SIZE + 10, WINDOW_OFFY + 50);
                        color_picker = glutCreateWindow("Select Color");
                        glutDisplayFunc(display_small);
                        glutMouseFunc(mouse_small);
                        glutMotionFunc(motion_cp);
                        glutKeyboardFunc(key_cp);
                        glutReshapeFunc(reshape_cp);
                        glClearColor(0.2, 0.2, 0.2, 0.0);
                    }
                    else {
                        for (auto crv : curves)
                            for (auto &p : crv->get_cpts())
                                if (p.clicked(x, y)) {
                                    coloring = &p;
                                    selected = crv;
                                }

                        if (selected) {
                            if (color_picker) {
                                glutDestroyWindow(color_picker);
                                color_picker = 0;
                            }

                            glutInitWindowSize(321, 5 + BCOLOR_BUFFER + 85 * (int)selected->get_cpts().size());
                            glutInitWindowPosition(WINDOW_OFFX + WINDOW_SIZE + 10, WINDOW_OFFY);
                            color_picker = glutCreateWindow("Select Colors");
                            glutDisplayFunc(display_big);
                            glutMouseFunc(mouse_big);
                            glutMotionFunc(motion_cp);
                            glutKeyboardFunc(key_cp);
                            glutReshapeFunc(reshape_cp);
                            glClearColor(0.2, 0.2, 0.2, 0.0);
                        }
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

void motion(int x, int y) {
    mouseX = x;
    mouseY = y;

    if (mouseLeftDown && moving) {
        moving->x = x;
        moving->y = y;
        glutPostRedisplay();
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH) - 1, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void save_file() {
    close_color_window(coloring);

    string filename;
    printf("save file: ");
    cin >> filename;
    ofstream file;
    file.open(filename);

    file << curves.size() << endl;
    for (auto &c : curves)
        file << *c;

    file << points.size() << endl;
    for (auto &p : points)
        file << p;

    file.close();
}

void load_file() {
    close_color_window(coloring);

    string filename;
    printf("load file: ");
    cin >> filename;
    ifstream file;
    file.open(filename);
    curves.clear();
    points.clear();

    unsigned int num_curves;
    file >> num_curves;

    for (unsigned int i = 0; i < num_curves; i++) {
        int type;
        file >> type;

        unsigned int degree, fidel, size;
        float param;
        file >> degree >> param >> fidel >> size;

        for (int j = 0; j < size; j++) {
            point<float> p;
            file >> p;
            points.push_back(p);
        }

        curve *crv = nullptr;
        switch (type) {
            case curve::lagrange:
                crv = new lagrange(points, param, fidel);
                break;
            case curve::bezier:
                crv = new bezier(points, fidel);
                break;
            case curve::bspline:
                crv = new bspline(points, degree, fidel);
                break;
            case curve::catmullrom:
                crv = new catmullrom(points, degree, param, fidel);
                break;

            default:
                printf("file error: invalid type\n");
                break;
        }
        if (crv)
            curves.push_back(crv);

        points.clear();
    }
    int size;
    file >> size;
    for (int j = 0; j < size; j++) {
        point<float> p;
        file >> p;
        points.push_back(p);
    }

    file.close();
}

void key(unsigned char c, int x, int y) {
    switch(c)
    {
        case 9: //tab
            break;

        case 13: //return
            break;

        case 8: //delete
            if (!drawImage && selected) {
                close_color_window(coloring);

                auto it = curves.begin();
                for (; *it != selected && it != curves.end(); it++);
                curves.erase(it);
                delete selected;
                selected = nullptr;
                glutSetWindow(diffuse_window);
                glutPostRedisplay();
            }
            break;

        case 127: //backspace
            if (!drawImage && moving){
                close_color_window(moving == coloring);

                auto it = points.begin();
                for (; &(*it) != moving && it != points.end(); it++);
                points.erase(it);
                moving = nullptr;
                glutPostRedisplay();
            }
            break;

        case ' ':
            drawImage = !drawImage;
            glutPostRedisplay();
            break;

        case '-':
            if (drawImage && smoothness > 25) {
                smoothness -= 25;
                glutPostRedisplay();
            }
            break;

        case '=':
            if (drawImage && smoothness < 100) {
                smoothness += 25;
                glutPostRedisplay();
            }
            break;

        case '_':
            if (drawImage && iter > 4) {
                iter /= 2;
                glutPostRedisplay();
            }
            break;

        case '+':
            if (drawImage && iter < 512) {
                iter *= 2;
                glutPostRedisplay();
            }
            break;

        { case 's':
            if (!drawImage && curves.size())
                save_file();
            break;

        case 'l':
            if (!drawImage) {
                load_file();
                glutPostRedisplay();
            }
            break; }

        { case '1':
            if (!drawImage && points.size() > 1) {
                close_color_window(coloring);
                curves.push_back(new lagrange(points));
                points.clear();
                glutPostRedisplay();
            }
            break;

        case '2':
            if (!drawImage && points.size() > 1) {
                close_color_window(coloring);
                curves.push_back(new bezier(points));
                points.clear();
                glutPostRedisplay();
            }
            break;

        case '3':
            if (!drawImage && points.size() > 1) {
                close_color_window(coloring);
                curves.push_back(new bspline(points));
                points.clear();
                glutPostRedisplay();
            }
            break;

        case '4':
            if (!drawImage && points.size() > 1) {
                close_color_window(coloring);
                curves.push_back(new catmullrom(points));
                points.clear();
                glutPostRedisplay();
            }
            break; }

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
    glutInitWindowPosition(WINDOW_OFFX, WINDOW_OFFY);
    diffuse_window = glutCreateWindow("Diffusion Curves");
    init();
    glutReshapeFunc (reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(key);
    glutMainLoop();
    return 0;
}
