#pragma once
#include "curve.h"

class lagrange : public curve {
public:
    lagrange(const list<point<float>>& cpts, const float p = 0.0, const ui f = 50) {
        c_points = vector<point<float>>(cpts.begin(), cpts.end());
        degree = (ui)c_points.size() - 1;
        parameterization = p;
        fidelity = f;
    }

    curvetype get_type() const { return curvetype::lagrange; }

    void draw(const bool drawPoints, const bool selected, const point<float>* sp) const {
        if (c_points.size() < 2)
            return;

        auto knots = generate_knots(c_points, degree, parameterization);

        vector<point<float>> curve;
        for (ui t = 0; t <= (float)fidelity * knots[degree]; t++)
            curve.push_back(neville(knots, (float)t/(float)fidelity));

        curve::draw(curve, drawPoints, selected, sp);
    }

private:
    point<float> neville(const vector<float>& knots, const float t) const {
        auto hash = map<pair<ui, ui>, point<float>>();
        auto p = curve::neville(degree, 0, knots, t, hash);
        return p;
    }
};
