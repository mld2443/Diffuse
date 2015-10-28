#ifndef bspline_h
#define bspline_h

#include "curve.h"

class bspline : public curve {
public:
    bspline(const list<point<float>*>& cpts, const unsigned int f = 50, const unsigned int d = 2) {
        c_points = vector<point<float>*>(cpts.begin(), cpts.end());
        degree = d;
        fidelity = f;
        set_uniform_param();
    }
    
    curvetype get_type() const { return curvetype::bspline; }
    
    void draw(const bool drawPoints) const {
        if (c_points.size() < 2)
            return;
        
        vector<point<float>> curve;
        for (unsigned int piece = 0; piece < c_points.size() - degree; piece++)
            for (float t = 0; t < fidelity; t++)
                curve.push_back(deboor(piece, ((float)t / (float)fidelity) + piece + degree - 1));
        
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
    point<float> deboor(const unsigned int d, const unsigned int begin, const float t, map<pair<unsigned int,unsigned int>,point<float>>& hash) const {
        if (d == 0)
            return *(c_points[begin]);
        
        auto p = pair<unsigned int, unsigned int>(d, begin);
        auto ii = hash.find(p);
        
        if (ii != hash.end())
            return ii->second;
        
        // return (left * (begin + degree - t) + right * (t - (d - 1 )) / d
        return hash[p] = ((deboor(d - 1, begin, t, hash) * (begin + degree - t)) + \
                          (deboor(d - 1, begin + 1, t, hash) * (t - (d + begin - 1)))) / d;
    }
    
    point<float> deboor(const unsigned int piece, const float t) const {
        auto hash = map<pair<unsigned int, unsigned int>, point<float>>();
        auto p = deboor(degree, piece, t, hash);
        return p;
    }
};

#endif /* bspline_h */
