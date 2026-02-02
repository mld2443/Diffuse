#include "catmullrom.h"


CatmullRomCurve::CatmullRomCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(2uz)
  , Interpolant(0.0)
{}

CatmullRomCurve::CatmullRomCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is)
  , Interpolant(is)
{}

const char* CatmullRomCurve::getName() const { return name; }

bool CatmullRomCurve::canIncDegree() const {
    return getDegree() < m_controlPoints.size() / 2uz;
}

std::vector<ControlPoint> CatmullRomCurve::evaluateCurve() const {
    auto knots = generateKnots();

    std::vector<ControlPoint> interpolated;
    std::size_t begin = 0uz;
    for (std::size_t piece = getDegree() - 1uz; piece < m_controlPoints.size() - getDegree(); ++piece, ++begin)
        for (std::size_t t = (knots[piece] * (float)m_fidelity); (float)t < knots[piece + 1uz] * (float)m_fidelity; ++t)
            interpolated.push_back(deboor(knots, begin, (float)t/(float)m_fidelity));
    interpolated.push_back(m_controlPoints[begin + getDegree() - 1uz]);

    return interpolated;
}

ControlPoint CatmullRomCurve::deboor(std::size_t d, std::size_t begin, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const {
    auto key = std::pair(d + getDegree(), begin);
    if (auto hashed = memo.find(key); hashed != memo.end())
        return hashed->second;

    if (d == 1u)
        return memo[key] =
            ((neville(getDegree() - 1, begin, knots, t, memo) * (knots[begin + getDegree()] - t)) +
             (neville(getDegree() - 1, begin + 1, knots, t, memo) * (t - knots[d + begin - 1]))) /
            (knots[begin + getDegree()] - knots[d + begin - 1]);

    return memo[key] =
        ((deboor(d - 1, begin, knots, t, memo) * (knots[begin + getDegree()] - t)) +
         (deboor(d - 1, begin + 1, knots, t, memo) * (t - knots[d + begin - 1]))) /
        (knots[begin + getDegree()] - knots[d + begin - 1]);
}

ControlPoint CatmullRomCurve::deboor(const std::vector<float>& knots, std::size_t piece, float t) const {
    std::map<std::pair<std::size_t, std::size_t>, ControlPoint> memo;
    return deboor(getDegree(), piece, knots, t, memo);
}
