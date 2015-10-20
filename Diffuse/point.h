#ifndef POINT_H
#define POINT_H

#include <OpenGL/gl.h>

#define POINTSIZE 5

template <int n>
class Point {
public:
    float data[n];
    
    Point() {
        memset(data, 0, sizeof(float)* n);
    }
    
    Point(float * d) {
        for(int i = 0; i < n; i++)
            data[i] = d[i];
    }
    
    Point* operator =(const Point& v)const {
        for(int i = 0 ; i < n; i++)
            data[i] = v.data[i];
        return &this;
    }
    
    Point operator +(const Point& v)const {
        Point<n> rvalue;
        for(int i = 0 ; i < n; i++)
            rvalue.data[i] = data[i] + v.data[i];
        
        return rvalue;
    }
    
    Point operator -(const Point& v)const {
        Point<n> rvalue;
        for(int i = 0 ; i < n; i++)
            rvalue.data[i] = data[i] - v.data[i];
        
        return rvalue;
    }
    
    Point operator *(float s)const {
        Point<n> rvalue;
        for(int i = 0 ; i < n; i++)
            rvalue.data[i] = data[i]* s;
        
        return rvalue;
    }
    
    Point operator /(float s)const {
        Point<n> rvalue;
        for(int i = 0 ; i < n; i++)
            rvalue.data[i] = data[i]/ s;
        
        return rvalue;
    }
    
    void operator +=(const Point& v) {
        for(int i = 0 ; i < n; i++)
            data[i] += v.data[i];
    }
    
    void operator -=(const Point& v) {
        for(int i = 0 ; i < n; i++)
            data[i] -= v.data[i];
    }
    
    void operator *=(float s) {
        for(int i = 0 ; i < n; i++)
            data[i] *= s;
    }
    
    void operator /=(float s) {
        for(int i = 0 ; i < n; i++)
            data[i] /= s;
    }
    
    float dot(const Point &v)const {
        float rvalue = 0;
        for(int i = 0 ; i < n; i++)
            rvalue += data[i] * v.data[i];
        
        return rvalue;
    }
    
    void zero(void) {
        for(int i = 0; i < n; i++)
            data[i] = 0;
    }
    
    void normalize(void) {
        float len = 0;
        int i;
        
        for(i = 0; i < n; i++)
            len += data[i] * data[i];
        
        len = sqrt(len);
        
        for(i = 0; i < n; i++)
            data[i] /= len;
    }
    
    float &operator[](int i) {
        return data[i];
    }
    
    void draw(void) { // assumes n is at least 2!
        float x = data[0], y = data[1];
        
        glBegin(GL_QUADS);
        glVertex2f(x - POINTSIZE / 2.0, y + POINTSIZE / 2.0); 
        glVertex2f(x - POINTSIZE / 2.0, y - POINTSIZE / 2.0); 
        glVertex2f(x + POINTSIZE / 2.0, y - POINTSIZE / 2.0); 
        glVertex2f(x + POINTSIZE / 2.0, y + POINTSIZE / 2.0); 
        glEnd();
    }
};

#endif