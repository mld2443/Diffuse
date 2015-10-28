#ifndef curve_h
#define curve_h

#include <OpenGL/gl.h>
#include <utility>
#include <vector>
#include <list>
#include <map>

#include "Point.h"

using namespace std;

class curve {
public:
    enum curvetype { lagrange, bezier, bspline, catmullrom };
    
    virtual curvetype get_type() const=0;
    virtual void draw(const bool drawPoints) const=0;
    virtual list<point<float>*>* elevate_degree() { return nullptr; }
    
    unsigned int get_degree() const { return degree; }
    unsigned int get_fidelity() const { return fidelity; }
    void set_fidelity(const unsigned int f) { fidelity = f; }
    
    void set_uniform_param() { parameterization = 0.0; }
    void set_centrip_param() { parameterization = 0.5; }
    void set_chordlen_param(){ parameterization = 1.0; }
    void set_unusual_param(const float param) { parameterization = param; }
    float get_param() const { return parameterization; }
    
    void degree_inc() {
        if (get_type() > bezier && degree < c_points.size() - 1)
            degree++;
    }
    void degree_dec() {
        if (get_type() > bezier && degree - 1)
            degree--;
    }
    
protected:
    vector<point<float>*> c_points;
    unsigned int degree, fidelity;
    float parameterization;
    
    static vector<float> generate_knots(const vector<point<float>*>& c_points, const unsigned int size, const float parameterization) {
        vector<float> knots;
        knots.push_back(0.0);
        for (unsigned int i = 0; i < size; i++)
            knots.push_back(knots.back() + pow((*c_points[i] - *c_points[i + 1]).abs(), parameterization));
        return knots;
    }
    
    point<float> neville(const unsigned int d, const unsigned int begin, const vector<float>& knots, const float t, map<pair<unsigned int,unsigned int>,point<float>>& hash) const {
        if (d == 0)
            return *(c_points[begin]);
        
        auto p = pair<unsigned int, unsigned int>(d, begin);
        auto ii = hash.find(p);
        
        if (ii != hash.end())
            return ii->second;
        
        return hash[p] = \
        ((neville(d - 1, begin, knots, t, hash) * (knots[begin + d] - t)) + \
         (neville(d - 1, begin + 1, knots, t, hash) * (t - knots[begin]))) / \
        (knots[begin + d] - knots[begin]);
    }
};

#endif /* curve_h */
