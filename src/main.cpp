#include "curves/lagrange.h"
#include "curves/bezier.h"
#include "curves/bspline.h"
#include "curves/catmullrom.h"
#include "colorpicker.h"
#include "diffuse.h"

#include <GL/freeglut.h>

#include <fstream> // ifstream, ofstream
#include <list>    // list
#include <memory>  // make_unique, unique_ptr
#include <print>   // println
#include <vector>  // vector


namespace {
    constexpr std::size_t WINDOW_SIZE = 512uz;
    constexpr std::size_t WINDOW_OFFX = 100uz;
    constexpr std::size_t WINDOW_OFFY = 100uz;

    std::size_t g_smoothness = 25uz, g_iterations = 512uz;
    bool g_diffusing = false;

    ControlPoint *g_moving = nullptr;
    f32v2 g_lastCursorPos;
    bool g_mouseLeftDown = false, g_mouseRightDown = false;

    std::vector<ControlPoint> g_points;
    std::list<std::unique_ptr<BaseCurve>> g_curves;
}

int g_diffuseWindow = 0, g_colorPickerHandle = 0;

ControlPoint *g_hoveredPoint = nullptr;
BaseCurve *g_selectedCurve = nullptr;


void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for (const auto &curve : ::g_curves)
        curve->draw(!::g_diffusing, curve.get() == ::g_selectedCurve, ::g_hoveredPoint);

    if (::g_diffusing) {
        glFlush();

        std::vector<RGB<GLfloat>> rawPixels(::WINDOW_SIZE * ::WINDOW_SIZE);

        glReadPixels(0, 0, ::WINDOW_SIZE, ::WINDOW_SIZE, GL_RGB, GL_FLOAT, rawPixels.data());

        rawPixels = ::pyramidDiffusion(Image<GLfloat>(rawPixels, ::WINDOW_SIZE, ::WINDOW_SIZE), ::g_smoothness).convertToRGB();

        glDrawPixels(::WINDOW_SIZE, ::WINDOW_SIZE, GL_RGB, GL_FLOAT, rawPixels.data());
        glFlush();
    } else {
        for (const auto &p : ::g_points)
            p.draw(&p == ::g_moving, &p == ::g_hoveredPoint);

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

void closeColorWindow(const bool condition) {
    if (condition) {
        glutDestroyWindow(::g_colorPickerHandle);
        ::g_colorPickerHandle = 0;
        ::g_hoveredPoint = nullptr;
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH) - 1, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void saveFile() {
    ::closeColorWindow(::g_hoveredPoint);

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

void loadFile() {
    ::closeColorWindow(::g_hoveredPoint);

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

            if (type == BezierCurve::NAME)
                ::g_curves.push_back(std::make_unique<BezierCurve>(file));
            else if (type == LagrangeCurve::NAME)
                ::g_curves.push_back(std::make_unique<LagrangeCurve>(file));
            else if (type == BSplineCurve::NAME)
                ::g_curves.push_back(std::make_unique<BSplineCurve>(file));
            else if (type == CatmullRomCurve::NAME)
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

        case 8: //delete/backspace
            if (!::g_diffusing && ::g_selectedCurve) {
                ::closeColorWindow(::g_hoveredPoint);

                if (g_curves.remove_if([&](const auto& elem){ return elem.get() == ::g_selectedCurve; }))
                    ::g_selectedCurve = nullptr;

                glutSetWindow(::g_diffuseWindow);
                glutPostRedisplay();
            }
            break;

        case 127: //backspace/delete
            if (!::g_diffusing && ::g_moving) {
                ::closeColorWindow(::g_moving == ::g_hoveredPoint);

                auto it = ::g_points.begin();
                for (; &(*it) != ::g_moving && it != ::g_points.end(); ++it);
                ::g_points.erase(it);
                ::g_moving = nullptr;
                glutPostRedisplay();
            }
            break;

        case ' ':
            ::g_diffusing = !::g_diffusing;
            glutPostRedisplay();
            break;

        case '-':
            if (::g_diffusing && ::g_smoothness > 25uz) {
                ::g_smoothness -= 25;
                glutPostRedisplay();
            }
            break;

        case '=':
            if (::g_diffusing && ::g_smoothness < 100uz) {
                ::g_smoothness += 25;
                glutPostRedisplay();
            }
            break;

        case '_':
            if (::g_diffusing && ::g_iterations > 4uz) {
                ::g_iterations /= 2;
                glutPostRedisplay();
            }
            break;

        case '+':
            if (::g_diffusing && ::g_iterations < 512uz) {
                ::g_iterations *= 2;
                glutPostRedisplay();
            }
            break;

        case 's':
            if (!::g_diffusing && ::g_curves.size())
                ::saveFile();
            break;

        case 'l':
            if (!::g_diffusing) {
                ::loadFile();
                glutPostRedisplay();
            }
            break;

        case '1':
            if (!::g_diffusing && ::g_points.size() > 1uz) {
                ::closeColorWindow(::g_hoveredPoint);
                ::g_curves.push_back(std::make_unique<BezierCurve>(std::move(::g_points)));
                ::g_points.clear();
                glutPostRedisplay();
            }
            break;

        case '2':
            if (!::g_diffusing && ::g_points.size() > 1uz) {
                ::closeColorWindow(::g_hoveredPoint);
                ::g_curves.push_back(std::make_unique<LagrangeCurve>(std::move(::g_points)));
                ::g_points.clear();
                glutPostRedisplay();
            }
            break;

        case '3':
            if (!::g_diffusing && ::g_points.size() > 1uz) {
                ::closeColorWindow(::g_hoveredPoint);
                ::g_curves.push_back(std::make_unique<BSplineCurve>(std::move(::g_points)));
                ::g_points.clear();
                glutPostRedisplay();
            }
            break;

        case '4':
            if (!::g_diffusing && ::g_points.size() > 1uz) {
                ::closeColorWindow(::g_hoveredPoint);
                ::g_curves.push_back(std::make_unique<CatmullRomCurve>(std::move(::g_points)));
                ::g_points.clear();
                glutPostRedisplay();
            }
            break;

        case 27: //escape
            if (::g_colorPickerHandle != 0) {
                closeColorWindow(true);
                glutSetWindow(::g_diffuseWindow);
            } else {
                glutDestroyWindow(::g_diffuseWindow);
                exit(0);
            }
            break;

        default:
            break;
    }
}

void selectCurve(f32v2 click) {
    for (auto& curve : ::g_curves)
        for (auto &point : curve->getControlPoints())
            if (point.clicked(click)) {
                ::g_hoveredPoint = &point;
                ::g_selectedCurve = curve.get();
            }
}

void mouse(int button, int state, int x, int y) {
    ::g_lastCursorPos = { static_cast<float>(x), static_cast<float>(y) };

    if (g_diffusing)
        return;

    switch(button) {
        case GLUT_LEFT_BUTTON:
            ::g_mouseLeftDown = state == GLUT_DOWN;
            ::g_moving = nullptr;

            if (::g_mouseLeftDown) {
                // Check if selecting unaffiliated point on canvas
                for (auto &point : ::g_points)
                    if (point.clicked(g_lastCursorPos))
                        ::g_moving = &point;

                // Check if selecting point on an existing curve
                if (!::g_moving)
                    for (auto& curve : ::g_curves)
                        for (auto &point : curve->getControlPoints())
                            if (point.clicked(g_lastCursorPos))
                                ::g_moving = &point;

                // Add new point to canvas
                if (!::g_moving) {
                    ::g_points.push_back(ControlPoint{g_lastCursorPos});
                    ::g_moving = &::g_points.back();
                }
            }
            glutPostRedisplay();
            break;

        case GLUT_RIGHT_BUTTON:
            ::g_mouseRightDown = state == GLUT_DOWN;

            if (::g_mouseRightDown) {
                ::g_selectedCurve = nullptr;
                if (::g_colorPickerHandle) {
                    glutDestroyWindow(::g_colorPickerHandle);
                    ::g_colorPickerHandle = 0;
                }

                for (auto& curve : ::g_curves)
                    for (auto &point : curve->getControlPoints())
                        if (point.clicked(::g_lastCursorPos)) {
                            ::g_hoveredPoint = &point;
                            ::g_selectedCurve = curve.get();
                        }

                if (::g_selectedCurve) {
                    if (::g_colorPickerHandle) {
                        glutDestroyWindow(::g_colorPickerHandle);
                        ::g_colorPickerHandle = 0;
                    }

                    const char* title = nullptr;
                    if (dynamic_cast<BezierCurve*>(::g_selectedCurve))
                        title = "Bezier Curve";
                    else if (dynamic_cast<LagrangeCurve*>(::g_selectedCurve))
                        title = "Lagrange Curve";
                    else if (dynamic_cast<BSplineCurve*>(::g_selectedCurve))
                        title = "B-Spline Curve";
                    else if (dynamic_cast<CatmullRomCurve*>(::g_selectedCurve))
                        title = "Catmull-Rom Curve";

                    glutInitWindowSize(321, 5uz + ColorPicker::BCOLOR_BUFFER + 85uz * ::g_selectedCurve->getControlPoints().size());
                    glutInitWindowPosition(glutGet(GLUT_WINDOW_X) + glutGet(GLUT_WINDOW_WIDTH) + 10, glutGet(GLUT_WINDOW_Y));
                    ::g_colorPickerHandle = glutCreateWindow(title);
                    glutDisplayFunc(ColorPicker::display);
                    glutMouseFunc(ColorPicker::mouse);
                    glutMotionFunc(ColorPicker::motion);
                    glutKeyboardFunc(::key);
                    glutReshapeFunc(ColorPicker::reshape);
                    glClearColor(0.2, 0.2, 0.2, 0.0);
                }

                glutPostWindowRedisplay(::g_diffuseWindow);
            }
            break;
    }
}

void motion(int x, int y) {
    ::g_lastCursorPos = { static_cast<float>(x), static_cast<float>(y) };

    if (::g_mouseLeftDown && ::g_moving) {
        ::g_moving->coords.x = x;
        ::g_moving->coords.y = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(::WINDOW_SIZE, ::WINDOW_SIZE);
    glutInitWindowPosition(::WINDOW_OFFX, ::WINDOW_OFFY);
    ::g_diffuseWindow = glutCreateWindow("Diffusion Curves");
    ::init();
    glutReshapeFunc(::reshape);
    glutDisplayFunc(::display);
    glutMouseFunc(::mouse);
    glutMotionFunc(::motion);
    glutKeyboardFunc(::key);
    glutMainLoop();
    return 0;
}
