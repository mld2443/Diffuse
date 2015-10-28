#ifndef lagrange_h
#define lagrange_h

#include "curve.h"

class lagrange : public curve {
public:
    lagrange(const list<point<float>*>& cpts, const unsigned int f = 50.0) {
        c_points = vector<point<float>*>(cpts.begin(), cpts.end());
        degree = (unsigned int)c_points.size() - 1;
        fidelity = f;
        set_uniform_param();
    }
    
    curvetype get_type() const { return curvetype::bezier; }
    
    void draw(const bool drawPoints) const {
        if (c_points.size() < 2)
            return;
        
        auto knots = generate_knots(c_points, degree, parameterization);
        
        vector<point<float>> curve;
        for (unsigned int t = 0; t <= (float)fidelity * knots[degree]; t++)
            curve.push_back(neville(knots, (float)t/(float)fidelity));
        
        glBegin(GL_QUAD_STRIP); {
            auto i2 = curve.begin(), i1 = i2++;
            while (i2 != curve.end()) {
                auto left = i1->leftside(*i2);
                auto right = i1->rightside(*i2);
                
                glColor3ub(i1->lColor[0], i1->lColor[1], i1->lColor[2]);
                glVertex2f(left.x, left.y);
                glColor3ub(i1->rColor[0], i1->rColor[1], i1->rColor[2]);
                glVertex2f(right.x, right.y);
                
                i1++;
                i2++;
            }
            auto right = i1->leftside(*(i1 - 1));
            auto left = i1->rightside(*(i1 - 1));
            
            glColor3ub(i1->lColor[0], i1->lColor[1], i1->lColor[2]);
            glVertex2f(left.x, left.y);
            glColor3ub(i1->rColor[0], i1->rColor[1], i1->rColor[2]);
            glVertex2f(right.x, right.y);
            
        } glEnd();
        
        if (drawPoints)
            for (auto &p : c_points)
                p->draw(0);
        
    }
    
private:
    point<float> neville(const vector<float>& knots, const float t) const {
        auto hash = map<pair<unsigned int, unsigned int>, point<float>>();
        auto p = curve::neville(degree, 0, knots, t, hash);
        return p;
    }
};

#endif /* lagrange_h */
