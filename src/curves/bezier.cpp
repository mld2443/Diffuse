#include "bezier.h"

using namespace std;


BezierCurve::BezierCurve(std::vector<ControlPoint>&& controlPoints, std::size_t fidelity)
  : Curve(std::move(controlPoints), controlPoints.size() - 1uz, fidelity, 0.0f)
{}

Curve::CurveType BezierCurve::getType() const { return Curve::CurveType::bezier; }

std::vector<ControlPoint> BezierCurve::generateInterpolated() const {
    vector<ControlPoint> interpolated;
    for (uint32_t t = 0; t <= m_fidelity; t++)
        interpolated.push_back(decasteljau((float)t/(float)m_fidelity));

    return interpolated;
}

void BezierCurve::elevateDegree() {
    vector<ControlPoint> newpts;

    newpts.push_back(m_controlPoints.front());
    for (std::size_t i = 1uz; i < m_controlPoints.size(); ++i)
        newpts.push_back(m_controlPoints[i] * (1 - ((float)i / (float)(m_degree + 1))) + m_controlPoints[i - 1] * ((float)i / (float)(m_degree + 1)));
    newpts.push_back(m_controlPoints.back());

    m_controlPoints.clear();
    ++m_degree;
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
