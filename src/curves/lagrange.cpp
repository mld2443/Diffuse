#include "lagrange.h"

using namespace std;


LagrangeCurve::LagrangeCurve(const list<ControlPoint>& cpts, float p, uint32_t f) {
    c_points = vector<ControlPoint>(cpts.begin(), cpts.end());
    degree = (uint32_t)c_points.size() - 1ul;
    parameterization = p;
    fidelity = f;
}

Curve::CurveType LagrangeCurve::get_type() const { return Curve::CurveType::lagrange; }

void LagrangeCurve::draw(bool drawPoints, bool selected, const ControlPoint* sp) const {
    if (c_points.size() < 2ul)
        return;

    auto knots = generate_knots(c_points, degree, parameterization);

    vector<ControlPoint> curve;
    for (uint32_t t = 0u; t <= (float)fidelity * knots[degree]; t++)
        curve.push_back(neville(knots, (float)t/(float)fidelity));

    Curve::draw(curve, drawPoints, selected, sp);
}

ControlPoint LagrangeCurve::neville(const vector<float>& knots, float t) const {
    auto hash = map<pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = Curve::neville(degree, 0u, knots, t, hash);
    return p;
}
