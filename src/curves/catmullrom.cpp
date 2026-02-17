#include "catmullrom.h"
#include "../util/common.h"


CatmullRomCurve::CatmullRomCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(3uz, 2uz)
  , Parameterized(0.0f)
  , Interpolant()
{}

CatmullRomCurve::CatmullRomCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is, 2uz)
  , Parameterized(is)
  , Interpolant()
{}

const char* CatmullRomCurve::getName() const { return NAME; }

util::Range<std::size_t> CatmullRomCurve::getDomainIndices() const {
    return {deboorDegree(), m_controlPoints.size() - nevilleDegree()};
}

SplineCurve::KnotLayerBounds CatmullRomCurve::getKnotLayerBounds() const {
    return {}; //FIXME
}

std::size_t CatmullRomCurve::nevilleDegree() const {
    return (getDegree() + 1uz) >> 1uz;
}

std::size_t CatmullRomCurve::deboorDegree() const {
    return getDegree() >> 1uz;
}

std::vector<ControlPoint> CatmullRomCurve::nevilleLayer(const std::span<const ControlPoint>& lowerDegree, float t, const std::span<const float>& knots) const {
    const std::size_t degree = knots.size() - lowerDegree.size() + 1uz;

    return evaluateGenericLayer(lowerDegree, t, knots, 0uz, degree);
}

std::vector<ControlPoint> CatmullRomCurve::deboorLayer(const std::span<const ControlPoint>& lowerDegree, float t, const std::span<const float>& knots) const {
//  const std::size_t leftOffset = getDegree() + 1uz - lowerDegree.size();
//  const std::size_t rightOffset = getDegree();
    const std::size_t leftOffset = knots.size() - lowerDegree.size() + 1uz - nevilleDegree();
    const std::size_t rightOffset = nevilleDegree();

    return evaluateGenericLayer(lowerDegree, t, knots, leftOffset, rightOffset);
}
