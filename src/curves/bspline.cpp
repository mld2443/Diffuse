#include "bspline.h"

#include <cstddef>
#include <ranges> // concat, from_range, iota, transform


BSplineCurve::BSplineCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(2uz, 1uz)
  , Parameterized(0.0f)
  , Approximant()
{}

BSplineCurve::BSplineCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is, 1uz)
  , Parameterized(is)
  , Approximant()
{}

const char* BSplineCurve::getName() const { return NAME; }

util::Range<std::size_t> BSplineCurve::getDomainIndices() const {
    return { getDegree(), m_controlPoints.size() };
}

std::vector<float> BSplineCurve::generateKnots() const {
    // const int64_t knotCount = m_controlPoints.size() + getDegree() - 1l;
    // return {std::from_range, std::views::transform(std::views::iota((knotCount - 1l)/-2l, 1l + knotCount/2l), [](auto k){ return static_cast<float>(k); })};
    //return {std::from_range, std::views::iota(0uz, m_controlPoints.size() + getDegree() - 1uz)};
    const std::size_t segmentCount = m_controlPoints.size() - getDegree();
    return {std::from_range, std::views::concat(
        std::views::repeat(0.0f, getDegree()),
        std::views::transform(std::views::iota(0uz, segmentCount + 1uz), [](auto k){ return static_cast<float>(k); }),
        std::views::repeat(static_cast<float>(segmentCount), getDegree())
    )};
}

SplineCurve::KnotWindows BSplineCurve::getKnotWindows() const {
    const std::size_t degree = getDegree();

    return {std::from_range, std::views::transform(std::views::iota(1uz, degree + 1uz), [&](auto step){
        return util::Range{ step, degree + 1uz };
    })};
}
