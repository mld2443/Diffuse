#ifndef curve_h
#define curve_h

#include <OpenGL/gl.h>
#include <utility>
#include <vector>
#include <list>
#include <map>

#include "Point.h"

using namespace std;
typedef unsigned int ui;

class curve {
public:
    enum curvetype { lagrange, bezier, bspline, catmullrom };
    
    virtual curvetype get_type() const=0;
    virtual void draw(const bool drawPoints, const bool selected, const point<float>* sp) const=0;
    virtual void elevate_degree() { }
    
    ui get_degree() const { return degree; }
    ui get_fidelity() const { return fidelity; }
    float get_param() const { return parameterization; }
    vector<point<float>>& get_cpts() { return c_points; }
    
    void set_fidelity(const ui f) { fidelity = f; }
    
    void param_inc() {
        if (parameterization < 1.5)
            parameterization += 0.5;
    }
    void param_dec() {
        if (parameterization > 0)
            parameterization -= 0.5;
    }
    
    void degree_inc() {
        if (get_type() == bspline && degree < c_points.size() - 1)
            degree++;
        else if (get_type() == catmullrom && degree < c_points.size() / 2)
            degree++;
    }
    void degree_dec() {
        if (get_type() > bezier && degree - 1)
            degree--;
    }
    
protected:
    vector<point<float>> c_points;
    ui degree, fidelity;
    float parameterization;
    
    static vector<float> generate_knots(const vector<point<float>>& c_points, const ui size, const float parameterization) {
        vector<float> knots;
        knots.push_back(0.0);
        for (ui i = 0; i < size; i++)
            knots.push_back(knots.back() + pow((c_points[i] - c_points[i + 1]).abs(), parameterization));
        for (auto &knot : knots)
            knot = (knot/knots.back()) * (knots.size() - 1);
        return knots;
    }
    
    void draw(const vector<point<float>>& curve, const bool drawPoints, const bool selected, const point<float>* sp) const {
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
                p.draw(selected * ((&p == sp) + 1));
    }
    
    point<float> neville(const ui d, const ui begin, const vector<float>& knots, const float t, map<pair<ui, ui>, point<float>>& hash) const {
        if (d == 0)
            return c_points[begin];
        
        auto p = pair<ui, ui>(d, begin);
        auto ii = hash.find(p);
        
        if (ii != hash.end())
            return ii->second;
        
        return hash[p] = \
        ((neville(d - 1, begin, knots, t, hash) * (knots[begin + d] - t)) + \
         (neville(d - 1, begin + 1, knots, t, hash) * (t - knots[begin]))) / \
        (knots[begin + d] - knots[begin]);
    }
};

ostream& operator <<(ostream& os, curve& c){
    os << c.get_type() << endl;
    os << c.get_degree() << ' ' << c.get_param() << ' ' << c.get_fidelity() << ' ' << c.get_cpts().size() << endl;
    for (auto &pt : c.get_cpts())
        os << pt << endl;
    return os;
}

#endif /* curve_h */
