#include "colorpicker.h"
#include "curves/curve.h"
#include "curves/bezier.h"

#include <GL/freeglut.h>

#include <string>


extern int g_diffuseWindow, g_colorPickerHandle;

extern ControlPoint *g_coloring;
extern BaseCurve *g_selected;

namespace {
    bool g_mouseLeftDown;

    int g_selected_color;
}


template <typename... Args>
void ColorPicker::text(int x, int y, void* font, const std::format_string<Args...>& fmt, Args&&... args) {
    std::string formatted = std::format(fmt, std::forward<Args>(args)...);
    glRasterPos2i(x, y);
    for (const char character : formatted)
        glutBitmapCharacter(font, character);
}

void ColorPicker::displayCurveColors() {
    static const GLubyte available[] = {255,255,255};
    static const GLubyte disabled[]  = {128,128,128};

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glColor3ubv(dynamic_cast<SplineCurve*>(::g_selected) ? available : disabled);
    text(10, 18, GLUT_BITMAP_HELVETICA_10, "DEGREE:{}", ::g_selected->getDegree());
    glBegin(GL_LINES); {
        glVertex2i(74, 6);
        glVertex2i(74, 15);
        glVertex2i(70, 10);
        glVertex2i(79, 10);

        glVertex2i(70, 21);
        glVertex2i(79, 21);
    } glEnd();

    if (dynamic_cast<BezierCurve*>(::g_selected)) {
        glColor3ubv(available);
        text(110, 18, GLUT_BITMAP_HELVETICA_10, "ELEVATE DEGREE");
    }

    if (Interpolant* interp = dynamic_cast<Interpolant*>(::g_selected)) {
        glColor3ubv(available);
        text(230, 18, GLUT_BITMAP_HELVETICA_10, "PARAM:{:3}", interp->getParam());
        glBegin(GL_LINES); {
            glVertex2i(295, 6);
            glVertex2i(295, 15);
            glVertex2i(290, 10);
            glVertex2i(299, 10);

            glVertex2i(290, 21);
            glVertex2i(299, 21);
        } glEnd();
    }

    glPushMatrix(); {
        glTranslated(0, BCOLOR_BUFFER, 0);
        for (const auto &p : ::g_selected->getControlPoints()) {
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

                glColor3fv(&p.l.x);
                glVertex2i(276, 40);
                glVertex2i(276, 5);
                glVertex2i(311, 5);
                glVertex2i(311, 40);

                glColor3fv(&p.r.x);
                glVertex2i(276, 75);
                glVertex2i(276, 40);
                glVertex2i(311, 40);
                glVertex2i(311, 75);
            } glEnd();

            glBegin(GL_TRIANGLES); {
                glColor3ub(255,255,255);
                for (int i = 0; i < 3; ++i) {
                    const int lPos = (int)((&p.l.x)[i] * 255.0f);
                    glVertex2i(lPos + 6, i * 25 + 3);
                    glVertex2i(lPos + 14, i * 25 + 3);
                    glVertex2i(lPos + 10, i * 25 + 10);

                    const int rPos = (int)((&p.l.x)[i] * 255.0f);
                    glVertex2i(rPos + 10, i * 25 + 20);
                    glVertex2i(rPos + 14, i * 25 + 27);
                    glVertex2i(rPos + 6, i * 25 + 27);
                }
            } glEnd();
            glTranslatef(0, 85, 0);
        }
    } glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void ColorPicker::mouseCurveColors(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        ::g_mouseLeftDown = state == GLUT_DOWN;
        if (::g_mouseLeftDown) {
            if (y < BCOLOR_BUFFER) {
                if (SplineCurve* spline = dynamic_cast<SplineCurve*>(::g_selected)) {
                    if (x > 73 && x < 80) {
                        if (y > 5 && y < 16) {
                            spline->incDegree();
                            glutPostRedisplay();
                            glutSetWindow(g_diffuseWindow);
                            glutPostRedisplay();
                            glutSetWindow(g_colorPickerHandle);
                        }
                        else if (y > 16 && y < 26) {
                            spline->decDegree();
                            glutPostRedisplay();
                            glutSetWindow(g_diffuseWindow);
                            glutPostRedisplay();
                            glutSetWindow(g_colorPickerHandle);
                        }
                    }
                }
                if (BezierCurve* bezier = dynamic_cast<BezierCurve*>(::g_selected)) {
                    if (x > 109 && x < 202) {
                        if (y > 10 && y < 20) {
                            bezier->elevateDegree();
                            glutReshapeWindow(321, glutGet(GLUT_WINDOW_HEIGHT) + 85);
                            glutSetWindow(g_diffuseWindow);
                            glutPostRedisplay();
                            glutSetWindow(g_colorPickerHandle);
                        }
                    }
                }
                if (Interpolant* interp = dynamic_cast<Interpolant*>(::g_selected)) {
                    if (x > 289 && x < 300) {
                        if (y > 5 && y < 16) {
                            interp->paramInc();
                            glutPostRedisplay();
                            glutSetWindow(g_diffuseWindow);
                            glutPostRedisplay();
                            glutSetWindow(g_colorPickerHandle);
                        }
                        else if (y > 16 && y < 26) {
                            interp->paramDec();
                            glutPostRedisplay();
                            glutSetWindow(g_diffuseWindow);
                            glutPostRedisplay();
                            glutSetWindow(g_colorPickerHandle);
                        }
                    }
                }
            }
            else {
                ::g_selected_color = 0;
                auto it = ::g_selected->getControlPoints().begin();
                for (int i = BCOLOR_BUFFER; i < glutGet(GLUT_WINDOW_HEIGHT) && ::g_selected_color == 0; i += 85, it++) {
                    if (y-i-3 > 0 && y-i-3 < 8 && abs(x - 10 - (int)(255.0f * it->l.x)) < 5)
                        ::g_selected_color = 1;
                    else if (y-i-20 > 0 && y-i-20 < 8 && abs(x - 10 - (int)(255.0f * it->r.x)) < 5)
                        ::g_selected_color = 2;
                    else if (y-i-28 > 0 && y-i-28 < 8 && abs(x - 10 - (int)(255.0f * it->l.y)) < 5)
                        ::g_selected_color = 3;
                    else if (y-i-45 > 0 && y-i-45 < 8 && abs(x - 10 - (int)(255.0f * it->r.y)) < 5)
                        ::g_selected_color = 4;
                    else if (y-i-53 > 0 && y-i-53 < 8 && abs(x - 10 - (int)(255.0f * it->l.z)) < 5)
                        ::g_selected_color = 5;
                    else if (y-i-70 > 0 && y-i-70 < 8 && abs(x - 10 - (int)(255.0f * it->r.z)) < 5)
                        ::g_selected_color = 6;
                }
                if (::g_selected_color)
                    ::g_coloring = &*(--it);
            }
        }
    }
}

void ColorPicker::displayPointColor() {
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

        glColor3fv(&g_coloring->l.x);
        glVertex2i(10, 266);
        glVertex2i(10, 145);
        glVertex2i(138, 145);
        glVertex2i(138, 266);

        glColor3fv(&g_coloring->r.x);
        glVertex2i(138, 266);
        glVertex2i(138, 145);
        glVertex2i(266, 145);
        glVertex2i(266, 266);
    } glEnd();

    glBegin(GL_TRIANGLES); {
        glColor3ub(255,255,255);
        for (int i = 0; i < 3; ++i) {
            const int lPos = (int)((&g_coloring->l.x)[i] * 255.0f);
            glVertex2i(lPos + 6, i * 45 + 13);
            glVertex2i(lPos + 14, i * 45 + 13);
            glVertex2i(lPos + 10, i * 45 + 20);

            const int rPos = (int)((&g_coloring->r.x)[i] * 255.0f);
            glVertex2i(rPos + 10, i * 45 + 35);
            glVertex2i(rPos + 14, i * 45 + 42);
            glVertex2i(rPos + 6, i * 45 + 42);
        }
    } glEnd();

    glFlush();
    glutSwapBuffers();
}

void ColorPicker::mousePointColor(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        ::g_mouseLeftDown = state == GLUT_DOWN;
        if (::g_mouseLeftDown) {
            ::g_selected_color = 0;
            if (y-13 > 0 && y-13 < 8 && abs(x - 10 - (int)(255.0f * ::g_coloring->l.x)) < 5)
                ::g_selected_color = 1;
            else if (y-35 > 0 && y-35 < 8 && abs(x - 10 - (int)(255.0f * ::g_coloring->r.x)) < 5)
                ::g_selected_color = 2;
            else if (y-58 > 0 && y-58 < 8 && abs(x - 10 - (int)(255.0f * ::g_coloring->l.y)) < 5)
                ::g_selected_color = 3;
            else if (y-80 > 0 && y-80 < 8 && abs(x - 10 - (int)(255.0f * ::g_coloring->r.y)) < 5)
                ::g_selected_color = 4;
            else if (y-103 > 0 && y-103 < 8 && abs(x - 10 - (int)(255.0f * ::g_coloring->l.z)) < 5)
                ::g_selected_color = 5;
            else if (y-125 > 0 && y-125 < 8 && abs(x - 10 - (int)(255.0f * ::g_coloring->r.z)) < 5)
                ::g_selected_color = 6;
        }
    }
}

void ColorPicker::reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH) - 1, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

//void key(unsigned char c, int x, int y);

void ColorPicker::key(unsigned char c, int x, int y) {
    switch(c) {
        case 13: //return
            ::g_coloring = nullptr;
            ::g_selected = nullptr;
            glutDestroyWindow(g_colorPickerHandle);
            ::g_colorPickerHandle = 0;
            glutSetWindow(g_diffuseWindow);
            glutPostRedisplay();
            break;

        case 8:
            break;

        case 27: //escape
            glutDestroyWindow(g_colorPickerHandle);
            glutDestroyWindow(g_diffuseWindow);
            exit(0);
            break;
    }
}

void ColorPicker::motion(int x, int y) {
    if (::g_selected_color && ::g_mouseLeftDown) {
        const float setting = (x < 265 ? ((x > 10) ? (float)(x-10) : 0.0f) : 255.0f) / 255.0f;
        switch (::g_selected_color) {
            case 1:
                ::g_coloring->l.x = setting;
                break;
            case 2:
                ::g_coloring->r.x = setting;
                break;
            case 3:
                ::g_coloring->l.y = setting;
                break;
            case 4:
                ::g_coloring->r.y = setting;
                break;
            case 5:
                ::g_coloring->l.z = setting;
                break;
            case 6:
                ::g_coloring->r.z = setting;
                break;
        }

        glutPostRedisplay();
        glutSetWindow(::g_diffuseWindow);
        glutPostRedisplay();
        glutSetWindow(::g_colorPickerHandle);
    }
}
