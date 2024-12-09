#pragma once

#include <iostream>
#include <cstdint>


template <typename T>
struct V2 {
    T x, y;

    V2   operator-() const;
    V2   operator+(const V2& v) const;
    V2   operator-(const V2& v) const;
    V2   operator*(const V2& v) const;
    V2   operator/(const V2& v) const;
    V2   operator*(T s) const;
    V2   operator/(T s) const;
    V2&  operator=(const V2& v);
    V2& operator+=(const V2& v);
    V2& operator-=(const V2& v);
    V2& operator*=(const V2& v);
    V2& operator/=(const V2& v);
    V2& operator*=(T s);
    V2& operator/=(T s);

    T dot(const V2& v) const;

    T magnitudeSqr() const;
    T    magnitude() const;
    V2   direction() const;
};

template <typename T> std::istream& operator>>(std::istream& is, V2<T>& v);
template <typename T> std::ostream& operator<<(std::ostream& os, const V2<T>& v);


template <typename T>
struct V3 {
    T x, y, z;

    V3   operator-() const;
    V3   operator+(const V3& v) const;
    V3   operator-(const V3& v) const;
    V3   operator*(const V3& v) const;
    V3   operator/(const V3& v) const;
    V3   operator*(T s) const;
    V3   operator/(T s) const;
    V3&  operator=(const V3& v);
    V3& operator+=(const V3& v);
    V3& operator-=(const V3& v);
    V3& operator*=(const V3& v);
    V3& operator/=(const V3& v);
    V3& operator*=(T s);
    V3& operator/=(T s);

    T    dot(const V3& v) const;
    V3 cross(const V3& v) const;

    T magnitudeSqr() const;
    T    magnitude() const;
    V3   direction() const;
};

template <typename T> std::istream& operator>>(std::istream& is, V3<T>& v);
template <typename T> std::ostream& operator<<(std::ostream& os, const V3<T>& v);


template class V2<float>;   using f32v2 = V2<float>;   static_assert(sizeof(f32v2) == 2 * sizeof(float));
template class V2<int32_t>; using s32v2 = V2<int32_t>; static_assert(sizeof(s32v2) == 2 * sizeof(int32_t));
template class V3<float>;   using f32v3 = V3<float>;   static_assert(sizeof(f32v3) == 3 * sizeof(float));
template class V3<int32_t>; using s32v3 = V3<int32_t>; static_assert(sizeof(s32v3) == 3 * sizeof(int32_t));


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
