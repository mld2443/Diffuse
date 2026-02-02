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
V2<T> operator*(const T& lhs, const V2<T>& rhs) {
    return { lhs * rhs.x, lhs * rhs.y };
}

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

    template <typename SELF> constexpr SELF operator-(this const SELF&                  rhs) { return {       - rhs.x,       - rhs.y,       - rhs.z }; }
    template <typename SELF> constexpr SELF operator+(this const SELF& lhs, const SELF& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z }; }
    template <typename SELF> constexpr SELF operator-(this const SELF& lhs, const SELF& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z }; }
    template <typename SELF> constexpr SELF operator*(this const SELF& lhs, T           rhs) { return { lhs.x * rhs  , lhs.y * rhs  , lhs.z * rhs   }; }
    template <typename SELF> constexpr SELF operator/(this const SELF& lhs, T           rhs) { return { lhs.x / rhs  , lhs.y / rhs  , lhs.z / rhs   }; }
    template <typename SELF> constexpr SELF& operator+=(this SELF& lhs, const SELF& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; return lhs; }
    template <typename SELF> constexpr SELF& operator-=(this SELF& lhs, const SELF& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; return lhs; }
    template <typename SELF> constexpr SELF& operator*=(this SELF& lhs, T           rhs) { lhs.x *= rhs  ; lhs.y *= rhs  ; lhs.z *= rhs  ; return lhs; }
    template <typename SELF> constexpr SELF& operator/=(this SELF& lhs, T           rhs) { lhs.x /= rhs  ; lhs.y /= rhs  ; lhs.z /= rhs  ; return lhs; }

    template <typename SELF> constexpr T    dot(this const SELF& lhs, const SELF& rhs) { return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z; }
    template <typename SELF> constexpr SELF cross(this const SELF& lhs, const SELF& rhs) {
        return { lhs.y*rhs.z - lhs.z*rhs.y,
                 lhs.z*rhs.x - lhs.x*rhs.z,
                 lhs.x*rhs.y - lhs.y*rhs.x };
    }

    constexpr T magnitudeSqr(this const auto& self) { return self.dot(self); }
    constexpr T    magnitude(this const auto& self) { return std::hypot(self.x, self.y, self.z); }
    constexpr T          max(this const auto& self) { return std::max({ self.x, self.y, self.z }); }

    template <typename SELF> constexpr SELF normalized(this const SELF& self) { return self / self.magnitude(); }

    template <typename SELF> constexpr bool equals(this const SELF& lhs, const SELF& rhs, T epsilon = T(1.0e-6)) {
        return std::fabs(lhs.x - rhs.x) < epsilon
            && std::fabs(lhs.y - rhs.y) < epsilon
            && std::fabs(lhs.z - rhs.z) < epsilon;
    }
    template <typename SELF> constexpr bool operator==(this const SELF& lhs, const SELF& rhs) { return lhs.equals(rhs); }
};

template <typename T, template<typename> typename TYPE>// requires(TYPE<T>::x && TYPE<T>::y && TYPE<T>::z)
TYPE<T> operator*(const T& lhs, const TYPE<T>& rhs) {
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}

template <typename T>
std::istream& operator>>(std::istream& is, V3<T>& v) {
    return is >> v.x >> v.y >> v.z;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const V3<T>& v) {
    return os << v.x << " " << v.y << " " << v.z;
}


using f32v2 = V2<float>;
using i32v2 = V2<int32_t>;
using f32v3 = V3<float>;
using i32v3 = V3<int32_t>;


struct fColor : public f32v3 {
    //fColor(const char* desc);

    friend std::istream& operator>>(std::istream& is, fColor& v);
    friend std::ostream& operator<<(std::ostream& os, const fColor& v);
};


struct ControlPoint {
    f32v2 p;
    fColor l = { 0.3, 0.3, 0.3 }, r = { 0.7, 0.7, 0.7 };
    //fColor l = { "#4c4c4c" }, r = { "#B3B3B3" };

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

    void draw(bool onSelectedCurve, bool selectedPoint) const;

    ControlPoint leftside(const ControlPoint& o) const;
    ControlPoint rightside(const ControlPoint& o) const;

    bool clicked(const f32v2& clickPos) const;
};

ControlPoint  operator* (float lhs, const ControlPoint& rhs);
std::istream& operator>>(std::istream& is, ControlPoint& k);
std::ostream& operator<<(std::ostream& os, const ControlPoint& k);
