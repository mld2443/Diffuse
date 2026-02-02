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

std::vector<ControlPoint> LagrangeCurve::evaluateCurve() const {
    auto knots = generateKnots();

    std::vector<ControlPoint> interpolated;
    interpolated.reserve(static_cast<std::size_t>(static_cast<float>(m_fidelity) * knots.back()));
    for (float t = 0.0f; t <= static_cast<float>(m_fidelity) * knots.back(); t += 1.0f)
        interpolated.push_back(neville2(m_controlPoints, knots, t/static_cast<float>(m_fidelity)));

    return interpolated;
}
