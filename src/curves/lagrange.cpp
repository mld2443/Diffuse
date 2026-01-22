#include "lagrange.h"


LagrangeCurve::LagrangeCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , GlobalCurve()
  , Interpolant(0.0)
{}

LagrangeCurve::LagrangeCurve(std::istream& is)
  : BaseCurve(is)
  , GlobalCurve()
  , Interpolant(is)
{}

const char* LagrangeCurve::getName() const { return name; }

std::vector<ControlPoint> LagrangeCurve::generateInterpolated() const {
    auto knots = generateKnots();

    std::vector<ControlPoint> interpolated;
    std::size_t degree = getDegree();
    for (uint32_t t = 0u; t <= (float)m_fidelity * knots[degree]; ++t)
        interpolated.push_back(neville(knots, (float)t/(float)m_fidelity));

    return interpolated;
}

ControlPoint LagrangeCurve::neville(const std::vector<float>& knots, float t) const {
    std::map<std::pair<std::size_t, std::size_t>, ControlPoint> memo;
    return Interpolant::neville(getDegree(), 0u, knots, t, memo);
}
