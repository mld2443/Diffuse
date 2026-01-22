#include "lagrange.h"

using namespace std;


LagrangeCurve::LagrangeCurve(std::vector<ControlPoint>&& controlPoints)
  : Curve(std::move(controlPoints), controlPoints.size() - 1uz)
{}

Curve::CurveType LagrangeCurve::getType() const { return Curve::CurveType::lagrange; }

std::vector<ControlPoint> LagrangeCurve::generateInterpolated() const {
    auto knots = generateKnots(m_parameterization);

    vector<ControlPoint> interpolated;
    for (uint32_t t = 0u; t <= (float)m_fidelity * knots[m_degree]; ++t)
        interpolated.push_back(neville(knots, (float)t/(float)m_fidelity));

    return interpolated;
}

ControlPoint LagrangeCurve::neville(const vector<float>& knots, float t) const {
    auto hash = map<pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = Curve::neville(m_degree, 0u, knots, t, hash);
    return p;
}
