#include "bezier.h"


BezierCurve::BezierCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , GlobalCurve()
  , Approximant()
{}

BezierCurve::BezierCurve(std::istream& is)
  : BaseCurve(is)
  , GlobalCurve()
  , Approximant()
{}

const char* BezierCurve::getName() const { return name; }

std::vector<ControlPoint> BezierCurve::generateInterpolated() const {
    std::vector<ControlPoint> interpolated;
    for (std::size_t t = 0uz; t <= m_fidelity; ++t)
        interpolated.push_back(decasteljau(static_cast<float>(t)/static_cast<float>(m_fidelity)));

    return interpolated;
}

void BezierCurve::elevateDegree() {
    std::vector<ControlPoint> newpts;

    newpts.push_back(m_controlPoints.front());
    for (std::size_t i = 1uz; i < m_controlPoints.size(); ++i)
        newpts.push_back(m_controlPoints[i] * (1 - ((float)i / (float)(getDegree() + 1))) + m_controlPoints[i - 1] * ((float)i / (float)(getDegree() + 1)));
    newpts.push_back(m_controlPoints.back());

    m_controlPoints.clear();
    m_controlPoints = newpts;
}

ControlPoint BezierCurve::decasteljau(uint32_t d, uint32_t begin, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    if (d == 0)
        return m_controlPoints[begin];

    auto key = std::pair<uint32_t, uint32_t>(d, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    return hash[key] = (decasteljau(d - 1u, begin, t, hash) * (1.0f - t)) + (decasteljau(d - 1u, begin + 1u, t, hash) * t);
}

ControlPoint BezierCurve::decasteljau(float t) const {
    auto hash = std::map<std::pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = decasteljau(m_controlPoints.size() - 1ul, 0u, t, hash);
    return p;
}
