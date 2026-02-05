#include "bspline.h"


BSplineCurve::BSplineCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(2uz)
  , Parameterized(0.0f)
  , Approximant()
{}

BSplineCurve::BSplineCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is)
  , Parameterized(is)
  , Approximant()
{}

const char* BSplineCurve::getName() const { return NAME; }

bool BSplineCurve::canIncDegree() const {
    return getDegree() + 1uz < m_controlPoints.size();
}

ControlPoint BSplineCurve::deboor(float begin, float degree, float t) const {
    if (degree == 0.0f)
        return m_controlPoints[begin];

    return ((deboor(begin       , degree - 1.0f, t) * (begin + getDegree() - t  ))
          + (deboor(begin + 1.0f, degree - 1.0f, t) * (t - degree - begin + 1.0f)))
         / degree;
}

std::vector<ControlPoint> BSplineCurve::evaluateCurve() const {
    std::vector<ControlPoint> interpolated;
    for (std::size_t piece = 0uz; piece < m_controlPoints.size() - getDegree(); ++piece)
        for (float t = 0.0f; t < 50.0f; t += 1.0f)
            interpolated.push_back(deboor(piece, getDegree(), (t / 50.0f) + static_cast<float>(piece + getDegree() - 1uz)));

    return interpolated;
}
