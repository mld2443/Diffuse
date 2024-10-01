#include "curves/lagrange.h"
#include "curves/bezier.h"
#include "curves/bspline.h"
#include "curves/catmullrom.h"
#include "colorpicker.h"
#include "diffuse.h"

#include <GL/freeglut.h>
#include <fstream>
#include <list>

using namespace std;


#define WINDOW_SIZE 512
#define WINDOW_OFFX 100
#define WINDOW_OFFY 100

int g_diffuseWindow = 0, g_colorPickerHandle = 0;

int g_smoothness = 25, g_iter = 512;
list<ControlPoint> g_points;

ControlPoint *g_moving, *g_coloring;
std::list<Curve*> g_curves;
Curve *g_selected;

i32v2 g_cursorLastPos;

bool g_mouseLeftDown = false, g_mouseRightDown = false;
bool g_drawImage = false;


void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for (const auto &curve : g_curves)
        curve->draw(!g_drawImage, curve == g_selected, g_coloring);

    if (!g_drawImage) {
        for (const auto &p : g_points)
            p.draw((&p == g_moving) + (&p == g_coloring) * 2);
    }

    glFlush();

    if (g_drawImage) {
        GLubyte *pixels = new GLubyte[WINDOW_SIZE * WINDOW_SIZE * 3];
        glReadPixels(0, 0, WINDOW_SIZE, WINDOW_SIZE, GL_RGB, GL_UNSIGNED_BYTE, pixels);

        pixels = diffuse(WINDOW_SIZE, pixels, g_smoothness, g_iter);

        glDrawPixels(WINDOW_SIZE, WINDOW_SIZE, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glFlush();
        delete[] pixels;
    }

    glutSwapBuffers();
}

void init() {
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
        glutDestroyWindow(g_colorPickerHandle);
        g_colorPickerHandle = 0;
        g_coloring = nullptr;
    }
}

void mouse(int button, int state, int x, int y) {
    f32v2 clickPos{ (float)x, (float)y };

    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            g_mouseLeftDown = state == GLUT_DOWN;

            if (!g_drawImage){
                if (g_mouseLeftDown) {
                    g_moving = nullptr;
                    for (auto &p : g_points)
                        if (p.clicked(clickPos))
                            g_moving = &p;
                    if (!g_moving)
                        for (auto crv : g_curves)
                            for (auto &p : crv->get_cpts())
                                if (p.clicked(clickPos))
                                    g_moving = &p;

                    if (!g_moving) {
                        g_points.push_back(ControlPoint{ clickPos });
                    }
                }
                glutPostRedisplay();
            }
            break;
        case GLUT_RIGHT_BUTTON:
            g_mouseRightDown = state == GLUT_DOWN;

            if (!g_drawImage){
                if (g_mouseRightDown) {
                    g_coloring = nullptr;
                    g_selected = nullptr;
                    if (g_colorPickerHandle) {
                        glutDestroyWindow(g_colorPickerHandle);
                        g_colorPickerHandle = 0;
                    }

                    for (auto &p : g_points)
                        if (p.clicked(clickPos))
                            g_coloring = &p;

                    if (g_coloring) {
                        glutInitWindowSize(276, 276);
                        glutInitWindowPosition(WINDOW_OFFX + WINDOW_SIZE + 10, WINDOW_OFFY + 50);
                        g_colorPickerHandle = glutCreateWindow("Select Color");
                        glutDisplayFunc(ColorPicker::display_small);
                        glutMouseFunc(ColorPicker::mouse_small);
                        glutMotionFunc(ColorPicker::motion);
                        glutKeyboardFunc(ColorPicker::key);
                        glutReshapeFunc(ColorPicker::reshape);
                        glClearColor(0.2, 0.2, 0.2, 0.0);
                    }
                    else {
                        for (auto crv : g_curves)
                            for (auto &p : crv->get_cpts())
                                if (p.clicked(clickPos)) {
                                    g_coloring = &p;
                                    g_selected = crv;
                                }

                        if (g_selected) {
                            if (g_colorPickerHandle) {
                                glutDestroyWindow(g_colorPickerHandle);
                                g_colorPickerHandle = 0;
                            }

                            glutInitWindowSize(321, 5 + ColorPicker::BCOLOR_BUFFER + 85 * (int)g_selected->get_cpts().size());
                            glutInitWindowPosition(WINDOW_OFFX + WINDOW_SIZE + 10, WINDOW_OFFY);
                            g_colorPickerHandle = glutCreateWindow("Select Colors");
                            glutDisplayFunc(ColorPicker::display_big);
                            glutMouseFunc(ColorPicker::mouse_big);
                            glutMotionFunc(ColorPicker::motion);
                            glutKeyboardFunc(ColorPicker::key);
                            glutReshapeFunc(ColorPicker::reshape);
                            glClearColor(0.2, 0.2, 0.2, 0.0);
                        }
                    }
                    glutSetWindow(g_diffuseWindow);
                    glutPostRedisplay();
                }
            }
            break;
    }

    g_cursorLastPos = { x, y };
}

void motion(int x, int y) {
    g_cursorLastPos = { x, y };

    if (g_mouseLeftDown && g_moving) {
        g_moving->p.x = x;
        g_moving->p.y = y;
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
    close_color_window(g_coloring);

    string filename;
    printf("save file: ");
    cin >> filename;
    ofstream file;
    file.open(filename);

    file << g_curves.size() << endl;
    for (auto &c : g_curves)
        file << *c;

    file << g_points.size() << endl;
    for (auto &p : g_points)
        file << p;

    file.close();
}

void load_file() {
    close_color_window(g_coloring);

    string filename;
    printf("load file: ");
    cin >> filename;
    ifstream file;
    file.open(filename);
    g_curves.clear();
    g_points.clear();

    unsigned int num_curves;
    file >> num_curves;

    for (unsigned int i = 0; i < num_curves; i++) {
        int type;
        file >> type;

        unsigned int degree, fidel, size;
        float param;
        file >> degree >> param >> fidel >> size;

        for (int j = 0; j < size; j++) {
            ControlPoint p;
            file >> p;
            g_points.push_back(p);
        }

        Curve *crv = nullptr;
        switch (type) {
            case Curve::CurveType::lagrange:
                crv = new LagrangeCurve(g_points, param, fidel);
                break;
            case Curve::CurveType::bezier:
                crv = new BezierCurve(g_points, fidel);
                break;
            case Curve::CurveType::bspline:
                crv = new BSplineCurve(g_points, degree, fidel);
                break;
            case Curve::CurveType::catmullrom:
                crv = new CatmullRomCurve(g_points, degree, param, fidel);
                break;

            default:
                printf("file error: invalid type\n");
                break;
        }
        if (crv)
            g_curves.push_back(crv);

        g_points.clear();
    }
    int size;
    file >> size;
    for (int j = 0; j < size; j++) {
        ControlPoint p;
        file >> p;
        g_points.push_back(p);
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
            if (!g_drawImage && g_selected) {
                close_color_window(g_coloring);

                auto it = g_curves.begin();
                for (; *it != g_selected && it != g_curves.end(); it++);
                g_curves.erase(it);
                delete g_selected;
                g_selected = nullptr;
                glutSetWindow(g_diffuseWindow);
                glutPostRedisplay();
            }
            break;

        case 127: //backspace
            if (!g_drawImage && g_moving){
                close_color_window(g_moving == g_coloring);

                auto it = g_points.begin();
                for (; &(*it) != g_moving && it != g_points.end(); it++);
                g_points.erase(it);
                g_moving = nullptr;
                glutPostRedisplay();
            }
            break;

        case ' ':
            g_drawImage = !g_drawImage;
            glutPostRedisplay();
            break;

        case '-':
            if (g_drawImage && g_smoothness > 25) {
                g_smoothness -= 25;
                glutPostRedisplay();
            }
            break;

        case '=':
            if (g_drawImage && g_smoothness < 100) {
                g_smoothness += 25;
                glutPostRedisplay();
            }
            break;

        case '_':
            if (g_drawImage && g_iter > 4) {
                g_iter /= 2;
                glutPostRedisplay();
            }
            break;

        case '+':
            if (g_drawImage && g_iter < 512) {
                g_iter *= 2;
                glutPostRedisplay();
            }
            break;

        { case 's':
            if (!g_drawImage && g_curves.size())
                save_file();
            break;

        case 'l':
            if (!g_drawImage) {
                load_file();
                glutPostRedisplay();
            }
            break; }

        { case '1':
            if (!g_drawImage && g_points.size() > 1) {
                close_color_window(g_coloring);
                g_curves.push_back(new LagrangeCurve(g_points));
                g_points.clear();
                glutPostRedisplay();
            }
            break;

        case '2':
            if (!g_drawImage && g_points.size() > 1) {
                close_color_window(g_coloring);
                g_curves.push_back(new BezierCurve(g_points));
                g_points.clear();
                glutPostRedisplay();
            }
            break;

        case '3':
            if (!g_drawImage && g_points.size() > 1) {
                close_color_window(g_coloring);
                g_curves.push_back(new BSplineCurve(g_points));
                g_points.clear();
                glutPostRedisplay();
            }
            break;

        case '4':
            if (!g_drawImage && g_points.size() > 1) {
                close_color_window(g_coloring);
                g_curves.push_back(new CatmullRomCurve(g_points));
                g_points.clear();
                glutPostRedisplay();
            }
            break; }

        case 27: //escape
            glutDestroyWindow(g_diffuseWindow);
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
    g_diffuseWindow = glutCreateWindow("Diffusion Curves");
    init();
    glutReshapeFunc (reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(key);
    glutMainLoop();
    return 0;
}
