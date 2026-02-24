#include "bspline.h"

#include <cstddef>
#include <ranges> // concat, from_range, iota, transform


BSplineCurve::BSplineCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(2uz, 1uz)
  , Parameterized(0.0f)
  , Approximant()
  , m_clamped(false)
{}

BSplineCurve::BSplineCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is, 1uz)
  , Parameterized(is)
  , Approximant()
  , m_clamped(false)
{}

const char* BSplineCurve::getName() const { return NAME; }

bool BSplineCurve::getClamped() const { return m_clamped; }

void BSplineCurve::toggleClamped() { m_clamped = !m_clamped; }

util::Range<std::size_t> BSplineCurve::getDomainIndices() const {
    if (m_clamped) {
        return { getDegree(), m_controlPoints.size() };
    } else {
        return { getDegree() - 1uz, m_controlPoints.size() - 1uz };
    }
}

std::vector<float> BSplineCurve::generateKnots() const {
    // const int64_t knotCount = m_controlPoints.size() + getDegree() - 1l;
    // return {std::from_range, std::views::transform(std::views::iota((knotCount - 1l)/-2l, 1l + knotCount/2l), [](auto k){ return static_cast<float>(k); })};
    if (m_clamped) {
        const std::size_t segmentCount = m_controlPoints.size() - getDegree();
        return { std::from_range, std::views::concat(
            std::views::repeat(0.0f, getDegree()),
            std::views::transform(std::views::iota(0uz, segmentCount + 1uz), [](auto k){ return static_cast<float>(k); }),
            std::views::repeat(static_cast<float>(segmentCount), getDegree())
        ) };
    } else {
        return { std::from_range, std::views::iota(0uz, m_controlPoints.size() + getDegree() - 1uz) };
    }
}

SplineCurve::KnotWindows BSplineCurve::getKnotWindows() const {
    const std::size_t degree = getDegree() + (m_clamped ? 1uz : 0uz);

    return {std::from_range, std::views::transform(std::views::iota((m_clamped ? 1uz : 0uz), degree), [&](auto step){
        return util::Range{ step, degree };
    })};
}
