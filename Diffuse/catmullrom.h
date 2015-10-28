
#ifndef catmullrom_h
#define catmullrom_h

#include "curve.h"

class catmullrom : public curve {
public:
    catmullrom(const list<point<float>>& cpts, const ui d = 2, const float p = 0.0, const ui f = 50) {
        c_points = vector<point<float>>(cpts.begin(), cpts.end());
        degree = d;
        parameterization = p;
        fidelity = f;
    }
    
    curvetype get_type() const { return curvetype::catmullrom; }
    
    void draw(const bool drawPoints, const bool selected, const point<float>* sp) const {
        if (c_points.size() < 2)
            return;
        
        auto knots = generate_knots(c_points, (ui)c_points.size() - 1, parameterization);
        
        vector<point<float>> crv;
        int begin = 0;
        for (ui piece = degree - 1; piece < c_points.size() - degree; piece++) {
            for (ui t = (knots[piece] * (float)fidelity); (float)t < knots[piece + 1] * (float)fidelity; t++)
                crv.push_back(deboor(knots, begin, (float)t/(float)fidelity));
            begin++;
        }
        crv.push_back(c_points[begin+degree-1]);
        
        curve::draw(crv, drawPoints, selected, sp);
    }
    
private:
    point<float> deboor(const ui d, const ui begin, const vector<float>& knots, const float t, map<pair<ui, ui>, point<float>>& hash) const {
        auto p = pair<ui, ui>(d + degree, begin);
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
    
    point<float> deboor(const vector<float>& knots, const ui piece, const float t) const {
        auto hash = map<pair<ui, ui>, point<float>>();
        auto p = deboor(degree, piece, knots, t, hash);
        return p;
    }
};

#endif /* catmullrom_h */
