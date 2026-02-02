#include "point.h"

#include <GL/gl.h>
#include <cmath> // sqrt, abs

namespace {
    constexpr float LINEWIDTH = 3.0f;
}


////////////
// fColor //
////////////
std::istream& operator>>(std::istream& is, fColor& v) {
    auto toUnorm = [](int32_t from) {
        return static_cast<float>(from) / 255.0f;
    };

    int32_t x, y, z;
    is >> x >> y >> z;
    v = { toUnorm(x), toUnorm(y), toUnorm(z) };
    return is;
}

std::ostream& operator<<(std::ostream& os, const fColor& v) {
    auto toScalar = [](float from) {
        return static_cast<int32_t>(255.0f * from);
    };

    return os << toScalar(v.x) << " " << toScalar(v.y) << " " << toScalar(v.z);
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

void ControlPoint::draw(bool isHighlighted, bool inColoringWindow) const {
    // Highlighting
    glBegin(GL_QUADS); {
        if   (inColoringWindow) glColor3ub(235,  30,  30);
        else if (isHighlighted) glColor3ub(235, 235, 235);
        else                    glColor3ub( 30,  30,  30);

        glVertex2f(p.x - 1.0f - LINEWIDTH, p.y + 1.0f + LINEWIDTH);
        glVertex2f(p.x - 1.0f - LINEWIDTH, p.y - 1.0f - LINEWIDTH);
        glVertex2f(p.x + 1.0f + LINEWIDTH, p.y - 1.0f - LINEWIDTH);
        glVertex2f(p.x + 1.0f + LINEWIDTH, p.y + 1.0f + LINEWIDTH);
    } glEnd();

    glBegin(GL_QUADS); {
        glColor3fv(&l.x);
        glVertex2f(p.x - LINEWIDTH, p.y + LINEWIDTH);
        glVertex2f(p.x - LINEWIDTH, p.y - LINEWIDTH);
        glColor3fv(&r.x);
        glVertex2f(p.x + LINEWIDTH, p.y - LINEWIDTH);
        glVertex2f(p.x + LINEWIDTH, p.y + LINEWIDTH);
    } glEnd();
}

ControlPoint ControlPoint::leftside(const ControlPoint& c) const {
    auto tangent = (c.p - p).normalized();
    ControlPoint rvalue({ -tangent.y, tangent.x });

    return *this + rvalue * LINEWIDTH;
}

ControlPoint ControlPoint::rightside(const ControlPoint& c) const {
    auto tangent = (c.p - p).normalized();
    ControlPoint rvalue({ tangent.y, -tangent.x });

    return *this + rvalue * LINEWIDTH;
}

bool ControlPoint::clicked(const f32v2& clickPos) const {
    return std::abs(p.x - clickPos.x) < (2.0f * LINEWIDTH)
        && std::abs(p.y - clickPos.y) < (2.0f * LINEWIDTH);
}

ControlPoint operator* (float lhs, const ControlPoint& rhs) { return { .p = lhs * rhs.p, .l = { lhs * rhs.l }, .r = { lhs * rhs.r } }; }
std::istream& operator>>(std::istream& is,       ControlPoint& k) { return is >> k.p        >> k.l        >> k.r; }
std::ostream& operator<<(std::ostream& os, const ControlPoint& k) { return os << k.p << ' ' << k.l << ' ' << k.r; }
