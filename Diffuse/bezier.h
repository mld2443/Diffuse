#ifndef bezier_h
#define bezier_h

#include <OpenGL/gl.h>
#include <vector>
#include <list>
#include <map>

#include "point.h"

using namespace std;

class bezier {
private:
    vector<point<float>*> c_points;
    map<pair<unsigned int, unsigned int>,point<float>> hash;
    
    point<float> decasteljau(const unsigned int d, const unsigned int begin, const float t) {
        if (d == 0)
            return *(c_points[begin]);
        
        auto p = pair<unsigned int, unsigned int>(d, begin);
        auto index = hash.find(p);
        
        if (index != hash.end())
            return index->second;
        
        return hash[p] = (decasteljau(d - 1, begin, t) * (1.0 - t)) + (decasteljau(d - 1, begin + 1, t) * t);
    }
    
    point<float> decasteljau(const float t) {
        hash = map<pair<unsigned int, unsigned int>, point<float>>();
        auto p = decasteljau(c_points.size()-1, 0, t);
        hash.clear();
        return p;
    }

public:
    bezier(const list<point<float>*>& cpts): c_points(vector<point<float>*>(cpts.begin(), cpts.end())) { }
    
    void elevate_degree() {
        vector<point<float>*> newpts;
        
        newpts.push_back(c_points.front());
        for (int i = 1; i < c_points.size(); i++)
            newpts.push_back(new point<float>(*c_points[i] * (1 - ((float)i / (float)(c_points.size()))) + *c_points[i - 1] * ((float)i / (float)(c_points.size()))));
        newpts.push_back(c_points.back());
        
        c_points = newpts;
    }
    
    void draw(const unsigned int segments) {
        if (c_points.size() < 2)
            return;
        
        vector<point<float>> curve;
        for (unsigned int t = 0; t <= segments; t++)
            curve.push_back(decasteljau((float)t/(float)segments));
        
        glBegin(GL_QUAD_STRIP); {
            auto i2 = curve.begin(), i1 = i2++;
            while (i2 != curve.end()) {
                auto left = i1->leftside(*i2);
                auto right = i1->rightside(*i2);
                
                //glColor3f(1.0,1.0,1.0);
                glColor3ubv(i1->lColor);
                glVertex2f(left.x, left.y);
                glColor3ubv(i1->rColor);
                glVertex2f(right.x, right.y);
                
                i1++;
                i2++;
            }
            auto right = i1->leftside(*(i1 - 1));
            auto left = i1->rightside(*(i1 - 1));
            
            glColor3ubv(i1->lColor);
            glVertex2f(left.x, left.y);
            glColor3ubv(i1->rColor);
            glVertex2f(right.x, right.y);
            
        } glEnd();
    }
};

#endif /* bezier_h */
