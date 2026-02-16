#include "lagrange.h"


LagrangeCurve::LagrangeCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , GlobalCurve()
  , Parameterized(0.0f)
  , Interpolant()
{}

LagrangeCurve::LagrangeCurve(std::istream& is)
  : BaseCurve(is)
  , GlobalCurve()
  , Parameterized(is)
  , Interpolant()
{}

const char* LagrangeCurve::getName() const { return NAME; }

util::Range<float> LagrangeCurve::getDomain() const {
    return {0.0f, 1.0f};
}
