#pragma once

#include "curve.h"

class bezier : public curve {
public:
    bezier(const list<point<float>>& cpts, const ui f = 50) {
        c_points = vector<point<float>>(cpts.begin(), cpts.end());
        degree = (ui)c_points.size() - 1;
        parameterization = 0.0;
        fidelity = f;
    }

    curvetype get_type() const { return curvetype::bezier; }

    void draw(const bool drawPoints, const bool selected, const point<float>* sp) const {
        if (c_points.size() < 2)
            return;

        vector<point<float>> curve;
        for (ui t = 0; t <= fidelity; t++)
            curve.push_back(decasteljau((float)t/(float)fidelity));

        curve::draw(curve, drawPoints, selected, sp);
    }

    void elevate_degree() {
        auto newpts = vector<point<float>>();

        newpts.push_back(c_points.front());
        for (ui i = 1; i < c_points.size(); i++)
            newpts.push_back(c_points[i] * (1 - ((float)i / (float)(degree + 1))) + c_points[i - 1] * ((float)i / (float)(degree + 1)));
        newpts.push_back(c_points.back());

        c_points.clear();
        degree++;
        c_points = newpts;
    }

private:
    point<float> decasteljau(const ui d, const ui begin, const float t, map<pair<ui, ui>, point<float>>& hash) const {
        if (d == 0)
            return c_points[begin];

        auto p = pair<ui, ui>(d, begin);
        auto index = hash.find(p);

        if (index != hash.end())
            return index->second;

        return hash[p] = (decasteljau(d - 1, begin, t, hash) * (1.0 - t)) + (decasteljau(d - 1, begin + 1, t, hash) * t);
    }

    point<float> decasteljau(const float t) const {
        auto hash = map<pair<ui, ui>, point<float>>();
        auto p = decasteljau((ui)c_points.size()-1, 0, t, hash);
        return p;
    }
};
