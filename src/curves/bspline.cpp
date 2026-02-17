#include "bspline.h"
#include "../util/common.h"

#include <ranges>


BSplineCurve::BSplineCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(2uz, 1uz)
  , Parameterized(0.0f)
  , Approximant()
{}

BSplineCurve::BSplineCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is, 1uz)
  , Parameterized(0.0f)
  , Approximant()
{}

const char* BSplineCurve::getName() const { return NAME; }

util::Range<std::size_t> BSplineCurve::getDomainIndices() const {
    return {getDegree() - 1uz, m_controlPoints.size() - 1uz};
}

std::vector<float> BSplineCurve::generateKnots() const {
    return {std::from_range, std::ranges::iota_view(0uz, m_controlPoints.size() + getDegree() - 1uz)}; //FIXME
}

SplineCurve::KnotLayerBounds BSplineCurve::getKnotLayerBounds() const {
    return {}; //FIXME
}
