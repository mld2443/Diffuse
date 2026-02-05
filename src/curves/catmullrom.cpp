#include "catmullrom.h"


CatmullRomCurve::CatmullRomCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(2uz)
  , Interpolant(0.0)
  , m_nevilleSteps(1uz)
{}

CatmullRomCurve::CatmullRomCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is)
  , Interpolant(is)
  , m_nevilleSteps(1uz)
{}

const char* CatmullRomCurve::getName() const { return name; }

bool CatmullRomCurve::canIncDegree() const {
    return getDegree() < m_controlPoints.size() / 2uz;
}

ControlPoint CatmullRomCurve::neville2(std::size_t index, std::size_t degree, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const {
    if (degree == 0uz)
        return m_controlPoints[index];

    auto key = std::pair(degree, index);
    if (auto cached = memo.find(key); cached != memo.end())
        return cached->second;

    const float t_l = knots[index], t_r = knots[index + degree];
    return memo[key] = ((t_r - t  ) * neville2(index      , degree - 1uz, knots, t, memo)
                     +  (t   - t_l) * neville2(index + 1uz, degree - 1uz, knots, t, memo))
                     /  (t_r - t_l);
}

ControlPoint CatmullRomCurve::deboor(std::size_t index, std::size_t degree, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const {
    auto key = std::pair(degree + getDegree(), index);
    if (auto hashed = memo.find(key); hashed != memo.end())
        return hashed->second;

    const float t_l = knots[degree + index - 1uz], t_r = knots[index + getDegree()];

    if (degree <= m_nevilleSteps)
        return memo[key] = ((t_r - t  ) * neville2(index      , getDegree() - 1uz, knots, t, memo)
                         +  (t   - t_l) * neville2(index + 1uz, getDegree() - 1uz, knots, t, memo))
                         /  (t_r - t_l);

    return memo[key] = ((t_r - t  ) * deboor(index      , degree - 1uz, knots, t, memo)
                     +  (t   - t_l) * deboor(index + 1uz, degree - 1uz, knots, t, memo))
                     /  (t_r - t_l);
}

std::vector<ControlPoint> CatmullRomCurve::evaluateCurve() const {
    auto knots = generateKnots();

    std::vector<ControlPoint> interpolated;
    for (std::size_t index = 0uz, piece = getDegree() - 1uz; piece < m_controlPoints.size() - getDegree(); ++piece, ++index)
        for (std::size_t t = (knots[piece] * (float)m_fidelity); (float)t < knots[piece + 1uz] * (float)m_fidelity; ++t) {
            std::map<std::pair<std::size_t, std::size_t>, ControlPoint> memo;
            interpolated.push_back(deboor(index, getDegree(), knots, (float)t/(float)m_fidelity, memo));
        }
    interpolated.push_back(m_controlPoints[m_controlPoints.size() - getDegree()]);

    return interpolated;
}
