#include "catmullrom.h"
// #include "../util/common.h"


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

util::Range<float> CatmullRomCurve::getDomain() const {
    return {0.0f, 1.0f};
}

std::vector<float> CatmullRomCurve::generateKnots() const {
    return {};
}

// std::vector<ControlPoint> CatmullRomCurve::evaluateCurve() const {
//     const std::vector<float> knots{generateKnots()};
//     std::vector<ControlPoint> smoothCurve;
//     smoothCurve.reserve(m_fidelity + 1uz);
// 
//     const std::size_t nevilleSteps = nevilleDegree();
//     const std::size_t deboorSteps = deboorDegree();
// 
//     auto bounds = util::Range(knots[deboorSteps], knots[knots.size() - nevilleSteps]);
// 
//     for (std::size_t t = 0uz; t <= m_fidelity; ++t) {
//         const float normalized = static_cast<float>(t)/static_cast<float>(m_fidelity);
//         smoothCurve.push_back(evaluatePoint(knots, bounds.lerp(normalized)));
//     }
// 
//     return smoothCurve;
// }

std::size_t CatmullRomCurve::nevilleDegree() const {
    return (getDegree() + 1uz) >> 1uz;
}

std::size_t CatmullRomCurve::deboorDegree() const {
    return getDegree() >> 1uz;
}

std::vector<ControlPoint> CatmullRomCurve::nevilleLayer(const std::span<const ControlPoint>& lowerDegree, const std::span<const float>& knots, float t) const {
    const std::size_t layerDegree = knots.size() - lowerDegree.size() + 1uz;

    std::vector<ControlPoint> higherDegree;
    higherDegree.reserve(lowerDegree.size() - 1uz);

    using namespace std::ranges::views;
    for (const auto& [left, right, t_l, t_r] : zip(lowerDegree,
                                                   drop(lowerDegree, 1uz),
                                                   knots,
                                                   drop(knots, layerDegree)))
        higherDegree.push_back(((t_r - t  ) * left
                              + (t   - t_l) * right)
                              / (t_r - t_l));

    return higherDegree;
}

std::vector<ControlPoint> CatmullRomCurve::deboorLayer(const std::span<const ControlPoint>& lowerDegree, const std::span<const float>& knots, float t) const {
    const std::size_t leftOffset = knots.size() - lowerDegree.size() + 1uz - nevilleDegree();
    const std::size_t rightOffset = nevilleDegree();

    std::vector<ControlPoint> higherDegree;
    higherDegree.reserve(lowerDegree.size() - 1uz);

    using namespace std::ranges::views;
    for (const auto& [left, right, t_l, t_r] : zip(lowerDegree,
                                                   drop(lowerDegree, 1uz),
                                                   drop(knots, leftOffset),
                                                   drop(knots, rightOffset)))
        higherDegree.push_back(((t_r - t  ) * left
                              + (t   - t_l) * right)
                              / (t_r - t_l));

    return higherDegree;
}

// ControlPoint CatmullRomCurve::evaluatePoint(const std::vector<float>& knots, float t) const {
//     const std::size_t nevilleSteps = nevilleDegree();
//     const std::size_t start = std::min(getSegmentCount(), util::findIndexbetweenRanges(t, knots) - nevilleSteps);
// 
//     // collect relevant subset of points and knots
//     auto layer = std::ranges::to<std::vector<ControlPoint>>(std::ranges::views::counted(m_controlPoints.begin() + start, getDegree() + 1uz));
//     const auto knotsView = std::ranges::views::counted(knots.begin() + start, getDegree() + 1uz);
// 
//     while (layer.size() > nevilleSteps)
//         layer = nevilleLayer(layer, knotsView, t);
// 
//     while (layer.size() > 1uz)
//         layer = deboorLayer(layer, knotsView, t);
// 
//     return layer.front();
// }
