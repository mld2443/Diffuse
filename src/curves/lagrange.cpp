#include "lagrange.h"

using namespace std;


LagrangeCurve::LagrangeCurve(const list<ControlPoint>& cpts, float p, uint32_t f) {
    m_controlPoints = vector<ControlPoint>(cpts.begin(), cpts.end());
    m_degree = (uint32_t)m_controlPoints.size() - 1ul;
    m_parameterization = p;
    m_fidelity = f;
}

Curve::CurveType LagrangeCurve::getType() const { return Curve::CurveType::lagrange; }

void LagrangeCurve::draw(bool drawPoints, bool selected, const ControlPoint* sp) const {
    if (m_controlPoints.size() < 2ul)
        return;

    auto knots = generateKnots(m_controlPoints, m_degree, m_parameterization);

    vector<ControlPoint> curve;
    for (uint32_t t = 0u; t <= (float)m_fidelity * knots[m_degree]; t++)
        curve.push_back(neville(knots, (float)t/(float)m_fidelity));

    Curve::draw(curve, drawPoints, selected, sp);
}

ControlPoint LagrangeCurve::neville(const vector<float>& knots, float t) const {
    auto hash = map<pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = Curve::neville(m_degree, 0u, knots, t, hash);
    return p;
}
