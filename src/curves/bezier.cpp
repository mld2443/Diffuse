#include "bezier.h"

using namespace std;


BezierCurve::BezierCurve(const list<ControlPoint>& cpts, uint32_t f) {
    m_controlPoints = vector<ControlPoint>(cpts.begin(), cpts.end());
    m_degree = (uint32_t)m_controlPoints.size() - 1;
    m_parameterization = 0.0;
    m_fidelity = f;
}

Curve::CurveType BezierCurve::getType() const { return Curve::CurveType::bezier; }

void BezierCurve::draw(const bool drawPoints, const bool selected, const ControlPoint* sp) const {
    if (m_controlPoints.size() < 2)
        return;

    vector<ControlPoint> curve;
    for (uint32_t t = 0; t <= m_fidelity; t++)
        curve.push_back(decasteljau((float)t/(float)m_fidelity));

    Curve::draw(curve, drawPoints, selected, sp);
}

void BezierCurve::elevateDegree() {
    auto newpts = vector<ControlPoint>();

    newpts.push_back(m_controlPoints.front());
    for (uint32_t i = 1; i < m_controlPoints.size(); i++)
        newpts.push_back(m_controlPoints[i] * (1 - ((float)i / (float)(m_degree + 1))) + m_controlPoints[i - 1] * ((float)i / (float)(m_degree + 1)));
    newpts.push_back(m_controlPoints.back());

    m_controlPoints.clear();
    m_degree++;
    m_controlPoints = newpts;
}

ControlPoint BezierCurve::decasteljau(uint32_t d, uint32_t begin, float t, map<pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    if (d == 0)
        return m_controlPoints[begin];

    auto key = pair<uint32_t, uint32_t>(d, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    return hash[key] = (decasteljau(d - 1u, begin, t, hash) * (1.0f - t)) + (decasteljau(d - 1u, begin + 1u, t, hash) * t);
}

ControlPoint BezierCurve::decasteljau(float t) const {
    auto hash = map<pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = decasteljau(m_controlPoints.size() - 1ul, 0u, t, hash);
    return p;
}
