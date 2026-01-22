#include "curves/lagrange.h"
#include "curves/bezier.h"
#include "curves/bspline.h"
#include "curves/catmullrom.h"
#include "colorpicker.h"
#include "diffuse.h"

#include <GL/freeglut.h>

#include <fstream>
#include <list>
#include <memory>
#include <print>
#include <vector>


namespace {
    constexpr std::size_t WINDOW_SIZE = 512uz;
    constexpr std::size_t WINDOW_OFFX = 100uz;
    constexpr std::size_t WINDOW_OFFY = 100uz;

    std::size_t g_smoothness = 25uz, g_iterations = 512uz;
    bool g_drawImage = false;

    ControlPoint *g_moving = nullptr;
    s32v2 g_cursorLastPos;
    bool g_mouseLeftDown = false, g_mouseRightDown = false;

    std::vector<ControlPoint> g_points;
    std::list<std::unique_ptr<BaseCurve>> g_curves;
}

int g_diffuseWindow = 0, g_colorPickerHandle = 0;

ControlPoint *g_coloring = nullptr;
BaseCurve *g_selected = nullptr;


void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for (const auto &curve : ::g_curves)
        curve->draw(!::g_drawImage, curve.get() == ::g_selected, ::g_coloring);

    if (g_drawImage) {
        glFlush();

        std::vector<RGB<GLfloat>> rawPixels(::WINDOW_SIZE * ::WINDOW_SIZE);

        glReadPixels(0, 0, ::WINDOW_SIZE, ::WINDOW_SIZE, GL_RGB, GL_FLOAT, rawPixels.data());

        rawPixels = pyramidDiffusion(Image<GLfloat>(rawPixels, ::WINDOW_SIZE, ::WINDOW_SIZE), ::g_smoothness).convertToRGB();

        glDrawPixels(::WINDOW_SIZE, ::WINDOW_SIZE, GL_RGB, GL_FLOAT, rawPixels.data());
        glFlush();
    } else {
        for (const auto &p : ::g_points)
            p.draw((&p == ::g_moving) + (&p == ::g_coloring) * 2);

        glFlush();
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
        glutDestroyWindow(::g_colorPickerHandle);
        ::g_colorPickerHandle = 0;
        ::g_coloring = nullptr;
    }
}

void mouse(int button, int state, int x, int y) {
    f32v2 clickPos{ (float)x, (float)y };

    switch(button) {
        case GLUT_LEFT_BUTTON:
            ::g_mouseLeftDown = state == GLUT_DOWN;

            if (!::g_drawImage) {
                if (::g_mouseLeftDown) {
                    ::g_moving = nullptr;
                    for (auto &p : ::g_points)
                        if (p.clicked(clickPos))
                            ::g_moving = &p;
                    if (!::g_moving)
                        for (auto& curve : ::g_curves)
                            for (auto &p : curve->getControlPoints())
                                if (p.clicked(clickPos))
                                    ::g_moving = &p;

                    if (!::g_moving) {
                        ::g_points.push_back(ControlPoint{ clickPos });
                    }
                }
                glutPostRedisplay();
            }
            break;
        case GLUT_RIGHT_BUTTON:
            ::g_mouseRightDown = state == GLUT_DOWN;

            if (!g_drawImage) {
                if (::g_mouseRightDown) {
                    ::g_coloring = nullptr;
                    ::g_selected = nullptr;
                    if (::g_colorPickerHandle) {
                        glutDestroyWindow(::g_colorPickerHandle);
                        ::g_colorPickerHandle = 0;
                    }

                    for (auto &p : ::g_points)
                        if (p.clicked(clickPos))
                            ::g_coloring = &p;

                    if (g_coloring) {
                        glutInitWindowSize(276, 276);
                        glutInitWindowPosition(::WINDOW_OFFX + ::WINDOW_SIZE + 10, ::WINDOW_OFFY + 50);
                        ::g_colorPickerHandle = glutCreateWindow("Select point color");
                        glutDisplayFunc(ColorPicker::displayPointColor);
                        glutMouseFunc(ColorPicker::mousePointColor);
                        glutMotionFunc(ColorPicker::motion);
                        glutKeyboardFunc(ColorPicker::key);
                        glutReshapeFunc(ColorPicker::reshape);
                        glClearColor(0.2, 0.2, 0.2, 0.0);
                    } else {
                        for (auto& curve : ::g_curves)
                            for (auto &p : curve->getControlPoints())
                                if (p.clicked(clickPos)) {
                                    ::g_coloring = &p;
                                    ::g_selected = curve.get();
                                }

                        if (::g_selected) {
                            if (::g_colorPickerHandle) {
                                glutDestroyWindow(::g_colorPickerHandle);
                                ::g_colorPickerHandle = 0;
                            }

                            const char* title = nullptr;
                            if (dynamic_cast<BezierCurve*>(::g_selected))
                                title = "Bezier Curve";
                            else if (dynamic_cast<LagrangeCurve*>(::g_selected))
                                title = "Lagrange Curve";
                            else if (dynamic_cast<BSplineCurve*>(::g_selected))
                                title = "B-Spline Curve";
                            else if (dynamic_cast<CatmullRomCurve*>(::g_selected))
                                title = "Catmull-Rom Curve";

                            glutInitWindowSize(321, 5uz + ColorPicker::BCOLOR_BUFFER + 85uz * ::g_selected->getControlPoints().size());
                            glutInitWindowPosition(::WINDOW_OFFX + ::WINDOW_SIZE + 10uz, ::WINDOW_OFFY);
                            ::g_colorPickerHandle = glutCreateWindow(title);
                            glutDisplayFunc(ColorPicker::displayCurveColors);
                            glutMouseFunc(ColorPicker::mouseCurveColors);
                            glutMotionFunc(ColorPicker::motion);
                            glutKeyboardFunc(ColorPicker::key);
                            glutReshapeFunc(ColorPicker::reshape);
                            glClearColor(0.2, 0.2, 0.2, 0.0);
                        }
                    }
                    glutSetWindow(::g_diffuseWindow);
                    glutPostRedisplay();
                }
            }
            break;
    }

    ::g_cursorLastPos = { x, y };
}

void motion(int x, int y) {
    ::g_cursorLastPos = { x, y };

    if (::g_mouseLeftDown && ::g_moving) {
        ::g_moving->p.x = x;
        ::g_moving->p.y = y;
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
    close_color_window(::g_coloring);

    std::string filename;
    std::cout << "save file: ";
    std::cin >> filename;
    std::ofstream file;
    file.open(filename);

    file << ::g_curves.size() << std::endl;
    for (auto &c : ::g_curves)
        file << *c;

    file << ::g_points.size() << std::endl;
    for (auto &p : ::g_points)
        file << p;

    file.close();
}

void load_file() {
    close_color_window(::g_coloring);

    std::string filename;
    std::cout << "load file: ";
    std::cin >> filename;
    if (std::ifstream file(filename); file.is_open()) {
        ::g_curves.clear();
        ::g_points.clear();

        std::size_t count;
        file >> count;

        for (std::size_t curveId = 0uz; curveId < count; ++curveId) {
            std::string type;
            file >> type;

            if (type == LagrangeCurve::name)
                ::g_curves.push_back(std::make_unique<LagrangeCurve>(file));
            else if (type == BezierCurve::name)
                ::g_curves.push_back(std::make_unique<BezierCurve>(file));
            else if (type == BSplineCurve::name)
                ::g_curves.push_back(std::make_unique<BSplineCurve>(file));
            else if (type == CatmullRomCurve::name)
                ::g_curves.push_back(std::make_unique<CatmullRomCurve>(file));
            else
                std::println("file error: invalid type");
        }
        file >> count;
        for (std::size_t pointId = 0uz; pointId < count; ++pointId) {
            ControlPoint p;
            file >> p;
            ::g_points.push_back(p);
        }

        file.close();
    }
}

void key(unsigned char c, int x, int y) {
    switch(c) {
        case 9: //tab
            break;

        case 13: //return
            break;

        case 8: //delete
            if (!::g_drawImage && ::g_selected) {
                close_color_window(::g_coloring);

                auto it = ::g_curves.begin();
                for (; it->get() != ::g_selected && it != ::g_curves.end(); it++);
                ::g_curves.erase(it);
                ::g_selected = nullptr;
                glutSetWindow(::g_diffuseWindow);
                glutPostRedisplay();
            }
            break;

        case 127: //backspace
            if (!::g_drawImage && ::g_moving){
                close_color_window(::g_moving == ::g_coloring);

                auto it = ::g_points.begin();
                for (; &(*it) != ::g_moving && it != ::g_points.end(); it++);
                ::g_points.erase(it);
                ::g_moving = nullptr;
                glutPostRedisplay();
            }
            break;

        case ' ':
            ::g_drawImage = !::g_drawImage;
            glutPostRedisplay();
            break;

        case '-':
            if (::g_drawImage && ::g_smoothness > 25) {
                ::g_smoothness -= 25;
                glutPostRedisplay();
            }
            break;

        case '=':
            if (::g_drawImage && ::g_smoothness < 100) {
                ::g_smoothness += 25;
                glutPostRedisplay();
            }
            break;

        case '_':
            if (::g_drawImage && ::g_iterations > 4) {
                ::g_iterations /= 2;
                glutPostRedisplay();
            }
            break;

        case '+':
            if (::g_drawImage && ::g_iterations < 512) {
                ::g_iterations *= 2;
                glutPostRedisplay();
            }
            break;

        { case 's':
            if (!::g_drawImage && ::g_curves.size())
                save_file();
            break;

        case 'l':
            if (!::g_drawImage) {
                load_file();
                glutPostRedisplay();
            }
            break; }

        { case '1':
            if (!::g_drawImage && ::g_points.size() > 1uz) {
                close_color_window(::g_coloring);
                ::g_curves.push_back(std::make_unique<LagrangeCurve>(std::move(::g_points)));
                ::g_points.clear();
                glutPostRedisplay();
            }
            break;

        case '2':
            if (!::g_drawImage && ::g_points.size() > 1uz) {
                close_color_window(::g_coloring);
                ::g_curves.push_back(std::make_unique<BezierCurve>(std::move(::g_points)));
                ::g_points.clear();
                glutPostRedisplay();
            }
            break;

        case '3':
            if (!::g_drawImage && ::g_points.size() > 1uz) {
                close_color_window(::g_coloring);
                ::g_curves.push_back(std::make_unique<BSplineCurve>(std::move(::g_points)));
                ::g_points.clear();
                glutPostRedisplay();
            }
            break;

        case '4':
            if (!::g_drawImage && ::g_points.size() > 1uz) {
                close_color_window(::g_coloring);
                ::g_curves.push_back(std::make_unique<CatmullRomCurve>(std::move(::g_points)));
                ::g_points.clear();
                glutPostRedisplay();
            }
            break; }

        case 27: //escape
            glutDestroyWindow(::g_diffuseWindow);
            exit(0);
            break;

        default:
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(::WINDOW_SIZE, ::WINDOW_SIZE);
    glutInitWindowPosition(::WINDOW_OFFX, ::WINDOW_OFFY);
    ::g_diffuseWindow = glutCreateWindow("Diffusion Curves");
    ::init();
    glutReshapeFunc (reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(key);
    glutMainLoop();
    return 0;
}
