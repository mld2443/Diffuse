#include "bspline.h"


BSplineCurve::BSplineCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(2uz)
  , Approximant()
{}

BSplineCurve::BSplineCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is)
  , Approximant()
{}

const char* BSplineCurve::getName() const { return name; }

bool BSplineCurve::canIncDegree() const {
    return getDegree() + 1uz < m_controlPoints.size();
}

std::vector<ControlPoint> BSplineCurve::evaluateCurve() const {
    std::vector<ControlPoint> interpolated;
    for (uint32_t piece = 0; piece < m_controlPoints.size() - getDegree(); piece++)
        for (float t = 0; t < m_fidelity; t++)
            interpolated.push_back(deboor(piece, ((float)t / (float)m_fidelity) + piece + getDegree() - 1));

    return interpolated;
}

ControlPoint BSplineCurve::deboor(std::size_t d, std::size_t begin, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const {
    if (d == 0)
        return m_controlPoints[begin];

    auto key = std::pair(d, begin);
    if (auto hashed = memo.find(key); hashed != memo.end())
        return hashed->second;

    return memo[key] = ((deboor(d - 1, begin, t, memo) * (begin + getDegree() - t)) +
                        (deboor(d - 1, begin + 1, t, memo) * (t - (d + begin - 1)))) / d;
}

ControlPoint BSplineCurve::deboor(std::size_t piece, float t) const {
    std::map<std::pair<std::size_t, std::size_t>, ControlPoint> memo;
    return deboor(getDegree(), piece, t, memo);
}
