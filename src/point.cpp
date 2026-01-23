#include "point.h"

#include <GL/gl.h>
#include <cmath> // sqrt, abs

#define POINTSIZE 3

using namespace std;


////////////
// fColor //
////////////
float fColor::toUnorm(int32_t from) { return static_cast<float>(from) / 255.0f; }
int32_t fColor::toInt(float from) { return static_cast<int32_t>(255.0f * from); }

std::istream& operator>>(std::istream& is, fColor& v) {
    i32v3 in;
    is >> in.x >> in.y >> in.z;
    v = { fColor::toUnorm(in.x), fColor::toUnorm(in.y), fColor::toUnorm(in.z) };
    return is;
}

std::ostream& operator<<(std::ostream& os, const fColor& v) {
    return os << fColor::toInt(v.x) << " " << fColor::toInt(v.y) << " " << fColor::toInt(v.z);
}


//////////////////
// ControlPoint //
//////////////////
ControlPoint   ControlPoint::operator+(const ControlPoint& c) const { return { p + c.p, l + c.l, r + c.r }; }
ControlPoint   ControlPoint::operator-(const ControlPoint& c) const { return { p - c.p, l - c.l, r - c.r }; }
ControlPoint   ControlPoint::operator*(float s) const               { return { p *   s, l *   s, r *   s }; }
ControlPoint   ControlPoint::operator/(float s) const               { return { p /   s, l /   s, r /   s }; }
ControlPoint&  ControlPoint::operator=(const ControlPoint& c) { p =  c.p; l =  c.l; r =  c.r; return *this; }
ControlPoint& ControlPoint::operator+=(const ControlPoint& c) { p += c.p; l += c.l; r += c.r; return *this; }
ControlPoint& ControlPoint::operator-=(const ControlPoint& c) { p -= c.p; l -= c.l; r -= c.r; return *this; }
ControlPoint& ControlPoint::operator*=(float s)               { p *=   s; l *=   s; r *=   s; return *this; }
ControlPoint& ControlPoint::operator/=(float s)               { p /=   s; l /=   s; r /=   s; return *this; }

float ControlPoint::dot(const ControlPoint& c) const { return p.dot(c.p); }

void ControlPoint::draw(int32_t selected) const {
    // outline
    glBegin(GL_QUADS); {
        if (selected > 1)
            glColor3ub(235,60,60);
        else if (selected == 1)
            glColor3ub(235,235,235);
        else
            glColor3ub(60,60,60);
        glVertex2f(p.x - 1 - POINTSIZE, p.y + 1 + POINTSIZE);
        glVertex2f(p.x - 1 - POINTSIZE, p.y - 1 - POINTSIZE);
        glVertex2f(p.x + 1 + POINTSIZE, p.y - 1 - POINTSIZE);
        glVertex2f(p.x + 1 + POINTSIZE, p.y + 1 + POINTSIZE);
    } glEnd();

    glBegin(GL_QUADS); {
        glColor3f(l.x, l.y, l.z);
        glVertex2f(p.x - POINTSIZE, p.y + POINTSIZE);
        glVertex2f(p.x - POINTSIZE, p.y - POINTSIZE);
        glColor3f(r.x, r.y, r.z);
        glVertex2f(p.x + POINTSIZE, p.y - POINTSIZE);
        glVertex2f(p.x + POINTSIZE, p.y + POINTSIZE);
    } glEnd();
}

ControlPoint ControlPoint::leftside(const ControlPoint& c) const {
    auto tangent = (c.p - p).normalized();
    ControlPoint rvalue({ -tangent.y, tangent.x });

    return *this + rvalue * 3.0;
}

ControlPoint ControlPoint::rightside(const ControlPoint& c) const {
    auto tangent = (c.p - p).normalized();
    ControlPoint rvalue({ tangent.y, -tangent.x });

    return *this + rvalue * 3;
}

bool ControlPoint::clicked(const f32v2& clickPos) const {
    return abs(p.x - clickPos.x) < (2 * POINTSIZE) && abs(p.y - clickPos.y) < (2 * POINTSIZE);
}

ControlPoint operator* (float lhs, const ControlPoint& rhs) { return { .p = lhs * rhs.p, .l = lhs * rhs.l, .r = lhs * rhs.r }; }
istream& operator>>(istream& is, ControlPoint& k)       { return is >> k.p        >> k.l        >> k.r; }
ostream& operator<<(ostream& os, const ControlPoint& k) { return os << k.p << ' ' << k.l << ' ' << k.r; }
