#include "catmullrom.h"


CatmullRomCurve::CatmullRomCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(2uz)
  , Parameterized(0.0f)
  , Interpolant()
  , m_nevilleSteps(1uz)
{}

CatmullRomCurve::CatmullRomCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is)
  , Parameterized(0.0f)
  , Interpolant()
  , m_nevilleSteps(1uz)
{}

const char* CatmullRomCurve::getName() const { return NAME; }

bool CatmullRomCurve::canIncDegree() const {
    return getDegree() < m_controlPoints.size() / 2uz;
}

// ControlPoint CatmullRomCurve::neville(std::size_t index, std::size_t degree, const std::vector<float>& knots, float t) const {
//     if (degree == 0uz)
//         return m_controlPoints[index];
// 
//     const float t_l = knots[index], t_r = knots[index + degree];
//     return ((t_r - t  ) * neville(index      , degree - 1uz, knots, t)
//           + (t   - t_l) * neville(index + 1uz, degree - 1uz, knots, t))
//          / (t_r - t_l);
// }
// 
// ControlPoint CatmullRomCurve::deboor(std::size_t index, std::size_t degree, const std::vector<float>& knots, float t) const {
//     if (degree <= m_nevilleSteps)
//         return ((neville(index      , getDegree() - 1uz, knots, t) * (knots[index + getDegree()] - t ))
//               + (neville(index + 1uz, getDegree() - 1uz, knots, t) * (t - knots[degree + index - 1uz])))
//              / (knots[index + getDegree()] - knots[index + degree - 1uz]);
// 
//     return ((deboor(index      , degree - 1uz, knots, t) * (knots[index + getDegree()] - t ))
//           + (deboor(index + 1uz, degree - 1uz, knots, t) * (t - knots[degree + index - 1uz])))
//          / (knots[index + getDegree()] - knots[degree + index - 1uz]);
// }
// 
// std::vector<ControlPoint> CatmullRomCurve::evaluateCurve() const {
//     auto knots = generateKnots();
// 
//     std::vector<ControlPoint> interpolated;
// 
//     for (std::size_t piece = getDegree() - 1uz; piece < m_controlPoints.size() - getDegree(); ++piece)
//         for (float t = knots[piece] * 50.0f; t < knots[piece + 1uz] * 50.0f; t += 1.0f)
//             interpolated.push_back(deboor(index, getDegree(), knots, t/50.0f));
// 
//     interpolated.push_back(m_controlPoints[m_controlPoints.size() - getDegree()]);
// 
//     return interpolated;
// }
