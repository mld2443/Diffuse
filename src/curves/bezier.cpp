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

ControlPoint BezierCurve::decasteljau(std::size_t d, std::size_t begin, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const {
    if (d == 0uz)
        return m_controlPoints[begin];

    auto key = std::pair(d, begin);
    if (auto hashed = memo.find(key); hashed != memo.end())
        return hashed->second;

    return memo[key] = (decasteljau(d - 1uz, begin, t, memo) * (1.0f - t)) + (decasteljau(d - 1uz, begin + 1uz, t, memo) * t);
}

ControlPoint BezierCurve::decasteljau(float t) const {
    std::map<std::pair<std::size_t, std::size_t>, ControlPoint> memo;
    return decasteljau(getDegree(), 0uz, t, memo);
}
