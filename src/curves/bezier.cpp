#include "bezier.h"

using namespace std;


BezierCurve::BezierCurve(const list<ControlPoint>& cpts, uint32_t f) {
    c_points = vector<ControlPoint>(cpts.begin(), cpts.end());
    degree = (uint32_t)c_points.size() - 1;
    parameterization = 0.0;
    fidelity = f;
}

Curve::CurveType BezierCurve::get_type() const { return Curve::CurveType::bezier; }

void BezierCurve::draw(const bool drawPoints, const bool selected, const ControlPoint* sp) const {
    if (c_points.size() < 2)
        return;

    vector<ControlPoint> curve;
    for (uint32_t t = 0; t <= fidelity; t++)
        curve.push_back(decasteljau((float)t/(float)fidelity));

    Curve::draw(curve, drawPoints, selected, sp);
}

void BezierCurve::elevate_degree() {
    auto newpts = vector<ControlPoint>();

    newpts.push_back(c_points.front());
    for (uint32_t i = 1; i < c_points.size(); i++)
        newpts.push_back(c_points[i] * (1 - ((float)i / (float)(degree + 1))) + c_points[i - 1] * ((float)i / (float)(degree + 1)));
    newpts.push_back(c_points.back());

    c_points.clear();
    degree++;
    c_points = newpts;
}

ControlPoint BezierCurve::decasteljau(uint32_t d, uint32_t begin, float t, map<pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    if (d == 0)
        return c_points[begin];

    auto key = pair<uint32_t, uint32_t>(d, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    return hash[key] = (decasteljau(d - 1u, begin, t, hash) * (1.0f - t)) + (decasteljau(d - 1u, begin + 1u, t, hash) * t);
}

ControlPoint BezierCurve::decasteljau(float t) const {
    auto hash = map<pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = decasteljau(c_points.size() - 1ul, 0u, t, hash);
    return p;
}
