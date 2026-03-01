#include "colorpicker.h"
#include "curves/bezier.h"
#include "curves/bspline.h"
#include "curves/curve.h"

#include <GL/freeglut.h>

#include <string> // string


extern int g_diffuseWindow, g_colorPickerHandle;

extern ControlPoint *g_hoveredPoint;
extern BaseCurve *g_selectedCurve;

namespace {
    bool g_mouseLeftDown;

    enum class ColorSelect {
        L_RED,
        R_RED,
        L_GRN,
        R_GRN,
        L_BLU,
        R_BLU,
        NONE
    };

    ColorSelect g_selectedColor = ColorSelect::NONE;
}


template <typename... Args>
void ColorPicker::text(int x, int y, void* font, const std::format_string<Args...>& fmt, Args&&... args) {
    std::string formatted = std::format(fmt, std::forward<Args>(args)...);
    glRasterPos2i(x, y);
    for (const char character : formatted)
        glutBitmapCharacter(font, character);
}

void ColorPicker::display() {
    static const GLubyte available[] = {255,255,255};
    static const GLubyte disabled[]  = {128,128,128};

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    const GLubyte *textColor = disabled, *incrColor = nullptr, *decrColor = nullptr;

    if (SplineCurve* spline = dynamic_cast<SplineCurve*>(::g_selectedCurve)) {
        textColor = available;
        incrColor = spline->canIncDegree() ? available : disabled;
        decrColor = spline->canDecDegree() ? available : disabled;
    }

    glColor3ubv(textColor);
    text(10, 18, GLUT_BITMAP_HELVETICA_10, "DEGREE:{}", ::g_selectedCurve->getDegree());
    if (incrColor && decrColor) {
        glBegin(GL_LINES); {
            glColor3ubv(incrColor);
            glVertex2i(74, 6);
            glVertex2i(74, 15);
            glVertex2i(70, 10);
            glVertex2i(79, 10);

            glColor3ubv(decrColor);
            glVertex2i(70, 21);
            glVertex2i(79, 21);
        } glEnd();
    }

    if (dynamic_cast<BezierCurve*>(::g_selectedCurve)) {
        glColor3ubv(available);
        text(110, 18, GLUT_BITMAP_HELVETICA_10, "ELEVATE DEGREE");
    }

    if (BSplineCurve* bspline = dynamic_cast<BSplineCurve*>(::g_selectedCurve)) {
        glColor3ubv(available);
        text(110, 18, GLUT_BITMAP_HELVETICA_10, "CLAMPED {}", bspline->getClamped() ? "ON" : "OFF");
    }

    if (Parameterized* curve = dynamic_cast<Parameterized*>(::g_selectedCurve)) {
        glColor3ubv(available);
        text(230, 18, GLUT_BITMAP_HELVETICA_10, "PARAM:{:3}", curve->getParam());
        glBegin(GL_LINES); {
            glColor3ubv(curve->canIncParam() ? available : disabled);
            glVertex2i(295, 6);
            glVertex2i(295, 15);
            glVertex2i(290, 10);
            glVertex2i(299, 10);

            glColor3ubv(curve->canDecParam() ? available : disabled);
            glVertex2i(290, 21);
            glVertex2i(299, 21);
        } glEnd();
    }

    {
        const auto knots = ::g_selectedCurve->generateKnots();
        const auto indices = ::g_selectedCurve->getDomainIndices();
        text(10, 38, GLUT_BITMAP_HELVETICA_18, "<{:n}>", knots);
        // if (SplineCurve* spline = dynamic_cast<SplineCurve*>(::g_selectedCurve)) {
        //     text(10, 58, GLUT_BITMAP_HELVETICA_10, "{} -> [{}, {}); {}", indices, knots[indices.lower], knots[indices.upper], spline->getKnotWindows());
        // } else {
        //     text(10, 58, GLUT_BITMAP_HELVETICA_10, "{} -> [{}, {})", indices, knots[indices.lower], knots[indices.upper]);
        // }
    }

    glPushMatrix(); {
        glTranslated(0, BCOLOR_BUFFER, 0);
        for (const auto &p : ::g_selectedCurve->getControlPoints()) {
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

                    const int rPos = (int)((&p.r.x)[i] * 255.0f);
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

void ColorPicker::mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        ::g_mouseLeftDown = state == GLUT_DOWN;
        if (::g_mouseLeftDown) {
            if (y < BCOLOR_BUFFER) {
                if (SplineCurve* spline = dynamic_cast<SplineCurve*>(::g_selectedCurve)) {
                    if (x > 69 && x < 80) {
                        if (y > 5 && y < 16) {
                            spline->incDegree();
                            glutPostWindowRedisplay(g_diffuseWindow);
                            glutPostRedisplay();
                            return;
                        } else if (y > 16 && y < 27) {
                            spline->decDegree();
                            glutPostWindowRedisplay(g_diffuseWindow);
                            glutPostRedisplay();
                            return;
                        }
                    }
                }

                if (BezierCurve* bezier = dynamic_cast<BezierCurve*>(::g_selectedCurve)) {
                    if (x > 109 && x < 202) {
                        if (y > 10 && y < 20) {
                            bezier->elevateDegree();
                            glutReshapeWindow(321, glutGet(GLUT_WINDOW_HEIGHT) + 85);
                            glutPostWindowRedisplay(g_diffuseWindow);
                            return;
                        }
                    }
                }

                if (BSplineCurve* bspline = dynamic_cast<BSplineCurve*>(::g_selectedCurve)) {
                    if (x > 109 && x < 197) {
                        if (y > 10 && y < 20) {
                            bspline->toggleClamped();
                            glutPostWindowRedisplay(g_diffuseWindow);
                            glutPostRedisplay();
                            return;
                        }
                    }
                }

                if (Parameterized* curve = dynamic_cast<Parameterized*>(::g_selectedCurve)) {
                    if (x > 289 && x < 300) {
                        if (y > 5 && y < 16) {
                            curve->paramInc();
                            glutPostWindowRedisplay(g_diffuseWindow);
                            glutPostRedisplay();
                            return;
                        } else if (y > 16 && y < 27) {
                            curve->paramDec();
                            glutPostWindowRedisplay(g_diffuseWindow);
                            glutPostRedisplay();
                            return;
                        }
                    }
                }
            } else {
                ::g_selectedColor = ColorSelect::NONE;
                auto it = ::g_selectedCurve->getControlPoints().begin();
                for (int i = BCOLOR_BUFFER; i < glutGet(GLUT_WINDOW_HEIGHT) && ::g_selectedColor == ColorSelect::NONE; i += 85, it++) {
                    if (y-i-3 > 0 && y-i-3 < 8 && abs(x - 10 - (int)(255.0f * it->l.x)) < 5)
                        ::g_selectedColor = ColorSelect::L_RED;
                    else if (y-i-20 > 0 && y-i-20 < 8 && abs(x - 10 - (int)(255.0f * it->r.x)) < 5)
                        ::g_selectedColor = ColorSelect::R_RED;
                    else if (y-i-28 > 0 && y-i-28 < 8 && abs(x - 10 - (int)(255.0f * it->l.y)) < 5)
                        ::g_selectedColor = ColorSelect::L_GRN;
                    else if (y-i-45 > 0 && y-i-45 < 8 && abs(x - 10 - (int)(255.0f * it->r.y)) < 5)
                        ::g_selectedColor = ColorSelect::R_GRN;
                    else if (y-i-53 > 0 && y-i-53 < 8 && abs(x - 10 - (int)(255.0f * it->l.z)) < 5)
                        ::g_selectedColor = ColorSelect::L_BLU;
                    else if (y-i-70 > 0 && y-i-70 < 8 && abs(x - 10 - (int)(255.0f * it->r.z)) < 5)
                        ::g_selectedColor = ColorSelect::R_BLU;
                }
                if (::g_selectedColor != ColorSelect::NONE)
                    ::g_hoveredPoint = &*(--it);
            }
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

void ColorPicker::motion(int x, int y) {
    if (::g_mouseLeftDown) {
        const float setting = (x < 265 ? ((x > 10) ? (float)(x-10) : 0.0f) : 255.0f) / 255.0f;
        switch (::g_selectedColor) {
            case ColorSelect::L_RED: ::g_hoveredPoint->l.x = setting; break;
            case ColorSelect::L_GRN: ::g_hoveredPoint->l.y = setting; break;
            case ColorSelect::L_BLU: ::g_hoveredPoint->l.z = setting; break;
            case ColorSelect::R_RED: ::g_hoveredPoint->r.x = setting; break;
            case ColorSelect::R_GRN: ::g_hoveredPoint->r.y = setting; break;
            case ColorSelect::R_BLU: ::g_hoveredPoint->r.z = setting; break;
            default: return;
        }

        glutPostWindowRedisplay(g_diffuseWindow);
        glutPostRedisplay();
    }
}
