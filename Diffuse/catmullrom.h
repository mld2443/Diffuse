
#ifndef catmullrom_h
#define catmullrom_h

#include "curve.h"

class catmullrom : public curve {
public:
    catmullrom(const list<point<float>*>& cpts, const unsigned int f = 50, const unsigned int d = 2) {
        c_points = vector<point<float>*>(cpts.begin(), cpts.end());
        degree = d;
        fidelity = f;
        set_uniform_param();
    }
    
    /*vector<Point>& generate(const vector<Point>& control_points = vector<Point>()) {
        if (control_points.size())
            c_points = control_points;
        curve.clear();
        
        auto knots = generate_ints(c_points, c_points.size() - 1, parameterization);
        
        float step = (knots[degree] / (float)degree) * fidelity;
        int begin = 0;
        for (unsigned int piece = degree - 1; piece < c_points.size() - degree; piece++) {
            for (float t = knots[piece]; t <= knots[piece + 1]; t += step)
                curve.push_back(cm_deboor(knots, begin, t));
            begin++;
        }
        
        return curve;
    }*/
    
    curvetype get_type() const { return curvetype::bezier; }
    
    void draw(const bool drawPoints) const {
        if (c_points.size() < 2)
            return;
        
        auto knots = generate_knots(c_points, (unsigned int)c_points.size() - 1, parameterization);
        
        vector<point<float>> curve;
        int begin = 0;
        for (unsigned int piece = degree - 1; piece < c_points.size() - degree; piece++) {
            for (unsigned int t = (knots[piece] * (float)fidelity); (float)t <= knots[piece + 1] * (float)fidelity; t++)
                curve.push_back(deboor(knots, begin, (float)t/(float)fidelity));
            begin++;
        }
        
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
    point<float> deboor(const unsigned int d, const unsigned int begin, const vector<float>& knots, const float t, map<pair<unsigned int,unsigned int>,point<float>>& hash) const {
        auto p = pair<unsigned int, unsigned int>(d + degree, begin);
        auto ii = hash.find(p);
        
        if (ii != hash.end())
            return ii->second;
        
        if (d == 1)
            return hash[p] = \
            ((neville(degree - 1, begin, knots, t, hash) * (knots[begin + degree] - t)) +    \
             (neville(degree - 1, begin + 1, knots, t, hash) * (t - knots[d + begin - 1]))) / \
             (knots[begin + degree] - knots[d + begin - 1]);
        
        return hash[p] = \
        ((deboor(d - 1, begin, knots, t, hash) * (knots[begin + degree] - t)) +       \
         (deboor(d - 1, begin + 1, knots, t, hash) * (t - knots[d + begin - 1]))) /    \
         (knots[begin + degree] - knots[d + begin - 1]);
    }
    
    point<float> deboor(const vector<float>& knots, const unsigned int piece, const float t) const {
        auto hash = map<pair<unsigned int, unsigned int>, point<float>>();
        auto p = deboor(degree, piece, knots, t, hash);
        return p;
    }
};

#endif /* catmullrom_h */
