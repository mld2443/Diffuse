#ifndef POINT_H
#define POINT_H

#include <OpenGL/gl.h>
#include <iostream>
#include <cmath>

#define POINTSIZE 3

template <class T>
class point {
public:
    T x, y;
    int lColor[3], rColor[3];
    
    point(const T _x=0.0, const T _y=0.0) {
        x = _x;
        y = _y;
        
        for (int i = 0; i < 3; i++){
            lColor[i] = 40;
            rColor[i] = 200;
        }
    }
    
    point operator =(const point& v) {
        x = v.x;
        y = v.y;
        
        for (int i = 0; i < 3; i++){
            lColor[i] = v.lColor[i];
            rColor[i] = v.rColor[i];
        }
        return *this;
    }
    
    point operator +(const point& v) const {
        point rvalue(v.x + x, v.y + y);
        
        for (int i = 0; i < 3; i++){
            rvalue.lColor[i] = lColor[i] + v.lColor[i];
            rvalue.rColor[i] = rColor[i] + v.rColor[i];
        }
        return rvalue;
    }
    
    point operator -(const point& v) const {
        point rvalue(v.x - x, v.y - y);
        
        for (int i = 0; i < 3; i++){
            rvalue.lColor[i] = lColor[i] - v.lColor[i];
            rvalue.rColor[i] = rColor[i] - v.rColor[i];
        }
        return rvalue;
    }
    
    point operator *(T s) const {
        point rvalue(s * x, s * y);
        
        for (int i = 0; i < 3; i++){
            rvalue.lColor[i] = lColor[i] * s;
            rvalue.rColor[i] = rColor[i] * s;
        }
        return rvalue;
    }
    
    point operator /(T s) const {
        point rvalue(x / s, y / s);
        
        for (int i = 0; i < 3; i++){
            rvalue.lColor[i] = lColor[i] / s;
            rvalue.rColor[i] = rColor[i] / s;
        }
        return rvalue;
    }
    
    void operator +=(const point& v) {
        x += v.x;
        y += v.y;
        
        for (int i = 0; i < 3; i++){
            lColor[i] += v.lColor[i];
            rColor[i] += v.rColor[i];
        }
    }
    
    void operator -=(const point& v) {
        x -= v.x;
        y -= v.y;
        
        for (int i = 0; i < 3; i++){
            lColor[i] -= v.lColor[i];
            rColor[i] -= v.rColor[i];
        }
    }
    
    void operator *=(const T s) {
        x *= s;
        y *= s;
        
        for (int i = 0; i < 3; i++){
            lColor[i] *= s;
            rColor[i] *= s;
        }
    }
    
    void operator /=(const T s) {
        x /= s;
        y /= s;
        
        for (int i = 0; i < 3; i++){
            lColor[i] /= s;
            rColor[i] /= s;
        }
    }
    
    T dot(const point &v)const {
        return (v.x*x + v.y*y);
    }
    
    void zero() {
        x = 0.0;
        y = 0.0;
        
        for (int i = 0; i < 3; i++){
            lColor[i] = 0;
            rColor[i] = 0;
        }
    }
    
    T abs() const {
        return sqrt(x * x + y * y);
    }
    
    void normalize() {
        T len = sqrt(x*x + y*y);
        x /= len;
        y /= len;
    }
    
    void draw(const int selected) const {
        // outline
        glBegin(GL_QUADS); {
            if (selected > 1)
                glColor3ub(235,60,60);
            else if (selected == 1)
                glColor3ub(235,235,235);
            else
                glColor3ub(60,60,60);
            glVertex2f(x - 1 - POINTSIZE, y + 1 + POINTSIZE);
            glVertex2f(x - 1 - POINTSIZE, y - 1 - POINTSIZE);
            glVertex2f(x + 1 + POINTSIZE, y - 1 - POINTSIZE);
            glVertex2f(x + 1 + POINTSIZE, y + 1 + POINTSIZE);
        } glEnd();
        
        glBegin(GL_QUADS); {
            glColor3ub(lColor[0], lColor[1], lColor[2]);
            glVertex2f(x - POINTSIZE, y + POINTSIZE);
            glVertex2f(x - POINTSIZE, y - POINTSIZE); 
            glColor3ub(rColor[0], rColor[1], rColor[2]);
            glVertex2f(x + POINTSIZE, y - POINTSIZE);
            glVertex2f(x + POINTSIZE, y + POINTSIZE);
        } glEnd();
    }
    
    point leftside(const point &v) const {
        point rvalue, tangent = v - *this;
        rvalue.x = -tangent.y;
        rvalue.y = tangent.x;
        
        rvalue.normalize();
        rvalue *= 3;
        
        return rvalue + *this;
    }
    
    point rightside(const point &v) const {
        point rvalue, tangent = v - *this;
        rvalue.x = tangent.y;
        rvalue.y = -tangent.x;
        
        rvalue.normalize();
        rvalue *= 3;
        
        return rvalue + *this;
    }
    
    bool clicked(const T mx, const T my) const {
        return (std::abs(x-mx) < (POINTSIZE*2) && std::abs(y-my) < (POINTSIZE*2));
    }
};

template <class T>
std::ostream& operator <<(std::ostream& os, const point<T> p){
    os << p.x << ' ' << p.y << ' ' << p.lColor[0] << ' ' << p.lColor[1] << ' ' << p.lColor[2] << ' ' << p.rColor[0] << ' ' << p.rColor[1] << ' ' << p.rColor[2];
    return os;
}

template <class T>
std::istream& operator >>(std::istream& is, point<T>& p){
    is >> p.x >> p.y >> p.lColor[0] >> p.lColor[1] >> p.lColor[2] >> p.rColor[0] >> p.rColor[1] >> p.rColor[2];
    return is;
}

#endif