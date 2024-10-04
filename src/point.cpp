#include "point.h"

#include <GL/gl.h>
#include <cmath>     // sqrt, abs
#include <algorithm> // max

#define POINTSIZE 3

using namespace std;


////////
// V2 //
////////
template <typename T> V2<T>   V2<T>::operator-() const               { return { -x     , -y      }; }
template <typename T> V2<T>   V2<T>::operator+(const V2<T>& v) const { return { x + v.x, y + v.y }; }
template <typename T> V2<T>   V2<T>::operator-(const V2<T>& v) const { return { x - v.x, y - v.y }; }
template <typename T> V2<T>   V2<T>::operator*(const V2<T>& v) const { return { x * v.x, y * v.y }; }
template <typename T> V2<T>   V2<T>::operator/(const V2<T>& v) const { return { x / v.x, y / v.y }; }
template <typename T> V2<T>   V2<T>::operator*(T s) const            { return { x *   s, y *   s }; }
template <typename T> V2<T>   V2<T>::operator/(T s) const            { return { x /   s, y /   s }; }
template <typename T> V2<T>&  V2<T>::operator=(const V2<T>& v) { x =  v.x;  y = v.y; return *this; }
template <typename T> V2<T>& V2<T>::operator+=(const V2<T>& v) { x += v.x; y += v.y; return *this; }
template <typename T> V2<T>& V2<T>::operator-=(const V2<T>& v) { x -= v.x; y -= v.y; return *this; }
template <typename T> V2<T>& V2<T>::operator*=(const V2<T>& v) { x *= v.x; y *= v.y; return *this; }
template <typename T> V2<T>& V2<T>::operator/=(const V2<T>& v) { x /= v.x; y /= v.y; return *this; }
template <typename T> V2<T>& V2<T>::operator*=(T s)            { x *=   s; y *=   s; return *this; }
template <typename T> V2<T>& V2<T>::operator/=(T s)            { x /=   s; y /=   s; return *this; }

template <typename T> T V2<T>::dot(const V2<T>& v) const { return x * v.x + y * v.y; }

template <typename T> T  V2<T>::magnitudeSqr() const { return this->dot(*this);     }
template <typename T> T     V2<T>::magnitude() const { return sqrt(magnitudeSqr()); }
template <typename T> V2<T> V2<T>::direction() const { return *this / magnitude();  }

template <typename T> istream& operator>>(istream& is, V2<T>& v)       { return is >> v.x        >> v.y; }
template <typename T> ostream& operator<<(ostream& os, const V2<T>& v) { return os << v.x << " " << v.y; }


////////
// V3 //
////////
template <typename T> V3<T>   V3<T>::operator-() const               { return { -x     , -y     , -z      }; }
template <typename T> V3<T>   V3<T>::operator+(const V3<T>& v) const { return { x + v.x, y + v.y, z + v.z }; }
template <typename T> V3<T>   V3<T>::operator-(const V3<T>& v) const { return { x - v.x, y - v.y, z - v.z }; }
template <typename T> V3<T>   V3<T>::operator*(const V3<T>& v) const { return { x * v.x, y * v.y, z * v.z }; }
template <typename T> V3<T>   V3<T>::operator/(const V3<T>& v) const { return { x / v.x, y / v.y, z / v.z }; }
template <typename T> V3<T>   V3<T>::operator*(T s) const            { return { x *   s, y *   s, z *   s }; }
template <typename T> V3<T>   V3<T>::operator/(T s) const            { return { x /   s, y /   s, z /   s }; }
template <typename T> V3<T>&  V3<T>::operator=(const V3<T>& v) { x =  v.x; y =  v.y; z =  v.z; return *this; }
template <typename T> V3<T>& V3<T>::operator+=(const V3<T>& v) { x += v.x; y += v.y; z += v.z; return *this; }
template <typename T> V3<T>& V3<T>::operator-=(const V3<T>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
template <typename T> V3<T>& V3<T>::operator*=(const V3<T>& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
template <typename T> V3<T>& V3<T>::operator/=(const V3<T>& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
template <typename T> V3<T>& V3<T>::operator*=(T s)            { x *=   s; y *=   s; z *=   s; return *this; }
template <typename T> V3<T>& V3<T>::operator/=(T s)            { x /=   s; y /=   s; z /=   s; return *this; }

template <typename T> T     V3<T>::  dot(const V3<T>& v) const { return x * v.x + y * v.y + z * v.z;                     }
template <typename T> V3<T> V3<T>::cross(const V3<T>& v) const { return { y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x }; }

template <typename T> T  V3<T>::magnitudeSqr() const { return this->dot(*this);     }
template <typename T> T     V3<T>::magnitude() const { return sqrt(magnitudeSqr()); }
template <typename T> V3<T> V3<T>::direction() const { return *this / magnitude();  }

template <typename T> istream& operator>>(istream& is, V3<T>& v)       { return is >> v.x        >> v.y        >> v.z; }
template <typename T> ostream& operator<<(ostream& os, const V3<T>& v) { return os << v.x << " " << v.y << " " << v.z; }


///////////
// Color //
///////////
float Color::toFloat(int32_t from) { return static_cast<float>(from) / 255.0f; }
int32_t Color::toInt(float from) { return static_cast<int32_t>(255.0f * from); }

std::istream& operator>>(std::istream& is, Color& v) {
    s32v3 in;
    is >> in.x >> in.y >> in.z;
    v = { Color::toFloat(in.x), Color::toFloat(in.y), Color::toFloat(in.z) };
    return is;
}

std::ostream& operator<<(std::ostream& os, const Color& v) {
    return os << Color::toInt(v.x) << " " << Color::toInt(v.y) << " " << Color::toInt(v.z);
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
    auto tangent = (c.p - p).direction();
    ControlPoint rvalue({ -tangent.y, tangent.x });

    return *this + rvalue * 3.0;
}

ControlPoint ControlPoint::rightside(const ControlPoint& c) const {
    auto tangent = (c.p - p).direction();
    ControlPoint rvalue({ tangent.y, -tangent.x });

    return *this + rvalue * 3;
}

bool ControlPoint::clicked(const f32v2& clickPos) const {
    return abs(p.x - clickPos.x) < (2 * POINTSIZE) && abs(p.y - clickPos.y) < (2 * POINTSIZE);
}

istream& operator>>(istream& is, ControlPoint& k)       { return is >> k.p        >> k.l        >> k.r; }
ostream& operator<<(ostream& os, const ControlPoint& k) { return os << k.p << ' ' << k.l << ' ' << k.r; }
