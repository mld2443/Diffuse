#include "point.h"

#include <GL/gl.h>


namespace {
    constexpr float LINEWIDTH = 3.0f;
}


////////////
// fColor //
////////////
std::istream& operator>>(std::istream& is, fColor& v) {
    auto toUnorm = [](uint32_t from) {
        return static_cast<float>(from) / 255.0f;
    };

    uint32_t x, y, z;
    is >> x >> y >> z;
    v = { toUnorm(x), toUnorm(y), toUnorm(z) };
    return is;
}

std::ostream& operator<<(std::ostream& os, const fColor& v) {
    auto toScalar = [](float from) {
        return static_cast<uint32_t>(255.0f * from);
    };

    return os << toScalar(v.x) << " " << toScalar(v.y) << " " << toScalar(v.z);
}


//////////////////
// ControlPoint //
//////////////////
ControlPoint   ControlPoint::operator+(const ControlPoint& c) const { return { coords + c.coords, l + c.l, r + c.r }; }
ControlPoint   ControlPoint::operator-(const ControlPoint& c) const { return { coords - c.coords, l - c.l, r - c.r }; }
ControlPoint   ControlPoint::operator*(float s) const               { return { coords *   s, l *   s, r *   s }; }
ControlPoint   ControlPoint::operator/(float s) const               { return { coords /   s, l /   s, r /   s }; }
ControlPoint&  ControlPoint::operator=(const ControlPoint& c) { coords =  c.coords; l =  c.l; r =  c.r; return *this; }
ControlPoint& ControlPoint::operator+=(const ControlPoint& c) { coords += c.coords; l += c.l; r += c.r; return *this; }
ControlPoint& ControlPoint::operator-=(const ControlPoint& c) { coords -= c.coords; l -= c.l; r -= c.r; return *this; }
ControlPoint& ControlPoint::operator*=(float s)               { coords *=   s; l *=   s; r *=   s; return *this; }
ControlPoint& ControlPoint::operator/=(float s)               { coords /=   s; l /=   s; r /=   s; return *this; }

float ControlPoint::dot(const ControlPoint& c) const { return coords.dot(c.coords); }

void ControlPoint::draw(bool isHighlighted, bool inColoringWindow) const {
    // Highlighting
    glBegin(GL_QUADS); {
        if   (inColoringWindow) glColor3ub(235,  30,  30);
        else if (isHighlighted) glColor3ub(235, 235, 235);
        else                    glColor3ub( 30,  30,  30);

        glVertex2f(coords.x - 1.0f - LINEWIDTH, coords.y + 1.0f + LINEWIDTH);
        glVertex2f(coords.x - 1.0f - LINEWIDTH, coords.y - 1.0f - LINEWIDTH);
        glVertex2f(coords.x + 1.0f + LINEWIDTH, coords.y - 1.0f - LINEWIDTH);
        glVertex2f(coords.x + 1.0f + LINEWIDTH, coords.y + 1.0f + LINEWIDTH);
    } glEnd();

    glBegin(GL_QUADS); {
        glColor3fv(&l.x);
        glVertex2f(coords.x - LINEWIDTH, coords.y + LINEWIDTH);
        glVertex2f(coords.x - LINEWIDTH, coords.y - LINEWIDTH);
        glColor3fv(&r.x);
        glVertex2f(coords.x + LINEWIDTH, coords.y - LINEWIDTH);
        glVertex2f(coords.x + LINEWIDTH, coords.y + LINEWIDTH);
    } glEnd();
}

f32v2 ControlPoint::leftside(const ControlPoint& c) const {
    auto tangent = (c.coords - coords).normalized();
    f32v2 rvalue({ -tangent.y, tangent.x });

    return coords + rvalue * LINEWIDTH;
}

f32v2 ControlPoint::rightside(const ControlPoint& c) const {
    auto tangent = (c.coords - coords).normalized();
    f32v2 rvalue({ tangent.y, -tangent.x });

    return coords + rvalue * LINEWIDTH;
}

bool ControlPoint::clicked(const f32v2& clickPos) const {
    return (coords - clickPos).magnitudeSqr() < (4.0f * LINEWIDTH * LINEWIDTH);
}

ControlPoint operator* (float lhs, const ControlPoint& rhs) { return { lhs * rhs.coords, lhs * rhs.l, lhs * rhs.r }; }
std::istream& operator>>(std::istream& is,       ControlPoint& k) { return is >> k.coords        >> k.l        >> k.r; }
std::ostream& operator<<(std::ostream& os, const ControlPoint& k) { return os << k.coords << ' ' << k.l << ' ' << k.r; }
