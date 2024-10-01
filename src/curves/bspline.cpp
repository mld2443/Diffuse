#include "bspline.h"

using namespace std;


BSplineCurve::BSplineCurve(const list<ControlPoint>& cpts, uint32_t d, uint32_t f) {
    c_points = vector<ControlPoint>(cpts.begin(), cpts.end());
    degree = d;
    parameterization = 0.0;
    fidelity = f;
}

Curve::CurveType BSplineCurve::get_type() const { return Curve::CurveType::bspline; }

void BSplineCurve::draw(bool drawPoints, bool selected, const ControlPoint* sp) const {
    if (c_points.size() < 2)
        return;

    vector<ControlPoint> curve;
    for (uint32_t piece = 0; piece < c_points.size() - degree; piece++)
        for (float t = 0; t < fidelity; t++)
            curve.push_back(deboor(piece, ((float)t / (float)fidelity) + piece + degree - 1));

    Curve::draw(curve, drawPoints, selected, sp);
}

ControlPoint BSplineCurve::deboor(uint32_t d, uint32_t begin, float t, map<pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    if (d == 0)
        return c_points[begin];

    auto key = pair<uint32_t, uint32_t>(d, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    return hash[key] = ((deboor(d - 1, begin, t, hash) * (begin + degree - t)) +
                        (deboor(d - 1, begin + 1, t, hash) * (t - (d + begin - 1)))) / d;
}

ControlPoint BSplineCurve::deboor(uint32_t piece, float t) const {
    auto hash = map<pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = deboor(degree, piece, t, hash);
    return p;
}
