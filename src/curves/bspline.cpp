#include "bspline.h"
// #include "../util/common.h"

// #include <algorithm> // min


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
    return {std::from_range, std::ranges::iota_view(0uz, m_controlPoints.size() + getDegree() - 1uz)};
}

// std::vector<ControlPoint> BSplineCurve::evaluateCurve() const {
//     const std::vector<float> knots{std::ranges::to<std::vector<float>>(std::ranges::iota_view(0uz, m_controlPoints.size() + getDegree() - 1uz))};
// 
//     std::vector<ControlPoint> smoothCurve;
//     smoothCurve.reserve(m_fidelity + 1uz);
// 
//     auto bounds = util::Range(knots[getDegree() - 1uz], knots[m_controlPoints.size() - 1uz]);
// 
//     for (std::size_t t = 0uz; t <= m_fidelity; ++t) {
//         const float normalized = static_cast<float>(t)/static_cast<float>(m_fidelity);
//         smoothCurve.push_back(evaluatePoint(knots, bounds.lerp(normalized)));
//     }
// 
//     return smoothCurve;
// }

// std::vector<ControlPoint> BSplineCurve::deboorLayer(const std::span<const ControlPoint>& lowerDegree, const std::span<const float>& knots, float t) const {
//     const std::size_t leftOffset = getDegree() + 1uz - lowerDegree.size();
//     const std::size_t rightOffset = getDegree();
// 
//     std::vector<ControlPoint> higherDegree;
//     higherDegree.reserve(lowerDegree.size() - 1uz);
// 
//     using namespace std::ranges::views;
//     for (const auto& [left, right, t_l, t_r] : zip(lowerDegree,
//                                                    drop(lowerDegree, 1uz),
//                                                    drop(knots, leftOffset),
//                                                    drop(knots, rightOffset)))
//         higherDegree.push_back(((t_r - t  ) * left
//                               + (t   - t_l) * right)
//                               / (t_r - t_l));
// 
//     return higherDegree;
// }

// ControlPoint BSplineCurve::evaluatePoint(const std::span<const float>& knots, float t) const {
//     const std::size_t start = std::min(getSegmentCount(), util::findIndexbetweenRanges(t, knots) - getDegree());
// 
//     // collect relevant subset of points and knots
//     auto layer = std::ranges::to<std::vector<ControlPoint>>(std::ranges::views::counted(m_controlPoints.begin() + start, getDegree() + 1uz));
// 
//     while (layer.size() > 1uz)
//         layer = deboorLayer(layer, std::ranges::views::drop(knots, start), t);
// 
//     return layer.front();
// }
