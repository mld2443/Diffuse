#ifndef POINT_H
#define POINT_H

#include <OpenGL/gl.h>
#include <cmath>

#define POINTSIZE 3

template <class T>
class point {
public:
    T x, y;
    GLubyte lColor[3], rColor[3];
    
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
    
    void normalize() {
        T len = sqrt(x*x + y*y);
        x /= len;
        y /= len;
    }
    
    void draw(const bool selected) {
        // outline
        glBegin(GL_QUADS); {
            if (selected)
                glColor3ub(215,215,215);
            else
                glColor3ub(40,40,40);
            glVertex2f(x - 1 - POINTSIZE, y + 1 + POINTSIZE);
            glVertex2f(x - 1 - POINTSIZE, y - 1 - POINTSIZE);
            glVertex2f(x + 1 + POINTSIZE, y - 1 - POINTSIZE);
            glVertex2f(x + 1 + POINTSIZE, y + 1 + POINTSIZE);
        } glEnd();
        
        glBegin(GL_QUADS); {
            glColor3ubv(lColor);
            glVertex2f(x - POINTSIZE, y + POINTSIZE);
            glVertex2f(x - POINTSIZE, y - POINTSIZE); 
            glColor3ubv(rColor);
            glVertex2f(x + POINTSIZE, y - POINTSIZE);
            glVertex2f(x + POINTSIZE, y + POINTSIZE);
        } glEnd();
    }
    
    void diff(const T dx, const T dy) {
        x -= dx;
        y -= dy;
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

#endif