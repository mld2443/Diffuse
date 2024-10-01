#pragma once

#include "curve.h"

class bspline : public curve {
public:
    bspline(const list<point<float>>& cpts, const ui d = 2, const ui f = 50) {
        c_points = vector<point<float>>(cpts.begin(), cpts.end());
        degree = d;
        parameterization = 0.0;
        fidelity = f;
    }

    curvetype get_type() const { return curvetype::bspline; }

    void draw(const bool drawPoints, const bool selected, const point<float>* sp) const {
        if (c_points.size() < 2)
            return;

        vector<point<float>> curve;
        for (ui piece = 0; piece < c_points.size() - degree; piece++)
            for (float t = 0; t < fidelity; t++)
                curve.push_back(deboor(piece, ((float)t / (float)fidelity) + piece + degree - 1));

        curve::draw(curve, drawPoints, selected, sp);
    }

private:
    point<float> deboor(const ui d, const ui begin, const float t, map<pair<ui, ui>, point<float>>& hash) const {
        if (d == 0)
            return c_points[begin];

        auto p = pair<ui, ui>(d, begin);
        auto ii = hash.find(p);

        if (ii != hash.end())
            return ii->second;

        return hash[p] = ((deboor(d - 1, begin, t, hash) * (begin + degree - t)) +
                          (deboor(d - 1, begin + 1, t, hash) * (t - (d + begin - 1)))) / d;
    }

    point<float> deboor(const ui piece, const float t) const {
        auto hash = map<pair<ui, ui>, point<float>>();
        auto p = deboor(degree, piece, t, hash);
        return p;
    }
};
