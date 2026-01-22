#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>


template <typename T>
struct V2 {
    T x, y;

    inline V2   operator-()            const { return { -x, -y };           }
    inline V2   operator+(const V2& v) const { return { x + v.x, y + v.y }; }
    inline V2   operator-(const V2& v) const { return { x - v.x, y - v.y }; }
    inline V2   operator*(T d)         const { return { x * d,   y * d };   }
    inline V2   operator/(T d)         const { return { x / d,   y / d };   }
    inline V2& operator+=(const V2& v) { x += v.x; y += v.y; return *this; }
    inline V2& operator-=(const V2& v) { x -= v.x; y -= v.y; return *this; }
    inline V2& operator*=(T d)         { x *= d;   y *= d;   return *this; }
    inline V2& operator/=(T d)         { x /= d;   y /= d;   return *this; }

    inline T    dot(const V2& v) const { return x*v.x + y*v.y; }

    inline T magnitudeSqr() const { return this->dot(*this);    }
    inline T    magnitude() const { return std::hypot(x, y);    }
    inline V2  normalized() const { return *this / magnitude(); }

    inline bool equals(const V2& v, T epsilon = T(1.0e-6)) const {
        return std::fabs(x - v.x) < epsilon &&
               std::fabs(y - v.y) < epsilon;
    }
    inline bool operator==(const V2& v) const { return equals(v); }

    inline T max() const { return std::max({ x, y }); }
};

template <typename T>
std::istream& operator>>(std::istream& is, V2<T>& v) {
    return is >> v.x >> v.y;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const V2<T>& v) {
    return os << v.x << " " << v.y;
}


template <typename T>
struct V3 {
    T x, y, z;

    inline V3   operator-()            const { return { -x, -y, -z };                }
    inline V3   operator+(const V3& v) const { return { x + v.x, y + v.y, z + v.z }; }
    inline V3   operator-(const V3& v) const { return { x - v.x, y - v.y, z - v.z }; }
    inline V3   operator*(T d)         const { return { x * d,   y * d,   z * d };   }
    inline V3   operator/(T d)         const { return { x / d,   y / d,   z / d };   }
    inline V3& operator+=(const V3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    inline V3& operator-=(const V3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    inline V3& operator*=(T d)         { x *= d;   y *= d;   z *= d;   return *this; }
    inline V3& operator/=(T d)         { x /= d;   y /= d;   z /= d;   return *this; }

    inline T    dot(const V3& v) const { return x*v.x + y*v.y + z*v.z;                           }
    inline V3 cross(const V3& v) const { return { y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x }; }

    inline T magnitudeSqr() const { return this->dot(*this);    }
    inline T    magnitude() const { return std::hypot(x, y, z); }
    inline V3  normalized() const { return *this / magnitude(); }

    inline bool equals(const V3& v, T epsilon = T(1.0e-6)) const {
        return std::fabs(x - v.x) < epsilon &&
               std::fabs(y - v.y) < epsilon &&
               std::fabs(z - v.z) < epsilon;
    }
    inline bool operator==(const V3& v) const { return equals(v); }

    inline T max() const { return std::max({ x, y, z }); }
};

template <typename T>
std::istream& operator>>(std::istream& is, V3<T>& v) {
    return is >> v.x >> v.y >> v.z;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const V3<T>& v) {
    return os << v.x << " " << v.y << " " << v.z;
}


using f32v2 = V2<float>;
using s32v2 = V2<int32_t>;
using f32v3 = V3<float>;
using s32v3 = V3<int32_t>;


class fColor : public f32v3 {
    static float toFloat(int32_t from);
    static int32_t toInt(float from);

    friend std::istream& operator>>(std::istream& is, fColor& v);
    friend std::ostream& operator<<(std::ostream& os, const fColor& v);
};


struct ControlPoint {
    f32v2 p;
    fColor l = { 0.3, 0.3, 0.3 }, r = { 0.7, 0.7, 0.7 };

    ControlPoint   operator+(const ControlPoint& o) const;
    ControlPoint   operator-(const ControlPoint& o) const;
    ControlPoint   operator*(float s) const;
    ControlPoint   operator/(float s) const;
    ControlPoint&  operator=(const ControlPoint& o);
    ControlPoint& operator+=(const ControlPoint& o);
    ControlPoint& operator-=(const ControlPoint& o);
    ControlPoint& operator*=(float s);
    ControlPoint& operator/=(float s);

    float dot(const ControlPoint& o) const;

    void draw(int32_t selected) const;

    ControlPoint leftside(const ControlPoint& o) const;
    ControlPoint rightside(const ControlPoint& o) const;

    bool clicked(const f32v2& clickPos) const;
};

std::istream& operator>>(std::istream& is, ControlPoint& k);
std::ostream& operator<<(std::ostream& os, const ControlPoint& k);
