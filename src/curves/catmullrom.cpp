#include "catmullrom.h"

#include <ranges>


namespace util {
    template <typename T>
    struct Range {
        T lower, upper;

        constexpr inline bool contains(const auto& value) const noexcept {
            if constexpr (requires{ std::is_floating_point_v<T>; })
                return lower <= value && value < upper;
            else
                return std::cmp_less_equal(lower, value) && std::cmp_less(value, upper);
        }
        constexpr inline T clamp(const T& value) const noexcept { return std::clamp<T>(value, lower, upper); }
        constexpr inline T lerp(auto t) const noexcept { return lower + t * (upper - lower); }
    };

    std::size_t findIndexFromRange(auto target, const std::ranges::range auto& sortedRange) {
        auto it = std::ranges::upper_bound(sortedRange, target);
        return static_cast<std::size_t>(std::distance(sortedRange.begin(), it));
    }
}


CatmullRomCurve::CatmullRomCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(3uz)
  , Interpolant(0.0)
{}

CatmullRomCurve::CatmullRomCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is)
  , Interpolant(is)
{}

const char* CatmullRomCurve::getName() const { return name; }

void CatmullRomCurve::incDegree() {
    m_degree += 2uz;
}

void CatmullRomCurve::decDegree() {
    m_degree -= 2uz;
}

ControlPoint CatmullRomCurve::imperativeNeville(std::size_t index, std::size_t degree, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const {
    if (degree == 0uz)
        return m_controlPoints[index];

    auto key = std::pair(degree, index);
    if (auto cached = memo.find(key); cached != memo.end())
        return cached->second;

    const float t_l = knots[index], t_r = knots[index + degree];
    return memo[key] = ((t_r - t  ) * imperativeNeville(index      , degree - 1uz, knots, t, memo)
                     +  (t   - t_l) * imperativeNeville(index + 1uz, degree - 1uz, knots, t, memo))
                     /  (t_r - t_l);
}

ControlPoint CatmullRomCurve::imperativeDeboor(std::size_t index, std::size_t degree, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const {
    auto key = std::pair(degree + getDegree(), index);
    if (auto hashed = memo.find(key); hashed != memo.end())
        return hashed->second;

    const float t_l = knots[index + degree - 1uz], t_r = knots[index + getDegree()];

    ControlPoint left, right;
    if (degree <= 1uz) {
        left  = imperativeNeville(index      , getDegree() - 1uz, knots, t, memo);
        right = imperativeNeville(index + 1uz, getDegree() - 1uz, knots, t, memo);
    } else {
        left  = imperativeDeboor(index      , degree - 1uz, knots, t, memo);
        right = imperativeDeboor(index + 1uz, degree - 1uz, knots, t, memo);
    }

    return memo[key] = ((t_r - t  ) * left
                     +  (t   - t_l) * right)
                     /  (t_r - t_l);
}

ControlPoint CatmullRomCurve::evaluatePointImperative(const std::vector<float>& knots, float t) const {
    std::map<std::pair<std::size_t, std::size_t>, ControlPoint> memo;
    return imperativeDeboor(util::findIndexFromRange(t, knots) - getDegree(), getDegree(), knots, t, memo);
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
    const std::size_t layerDegree = knots.size() - lowerDegree.size() + 1uz;
    const std::size_t nevilleDegree = (getDegree() + 1uz)/2uz;
    const std::size_t deboorDegree = layerDegree - nevilleDegree;

    std::vector<ControlPoint> higherDegree;
    higherDegree.reserve(lowerDegree.size() - 1uz);

    using namespace std::ranges::views;
    for (const auto& [left, right, t_l, t_r] : zip(lowerDegree,
                                                   drop(lowerDegree, 1uz),
                                                   drop(knots, deboorDegree),
                                                   drop(knots, nevilleDegree)))
        higherDegree.push_back(((t_r - t  ) * left + (t   - t_l) * right)
                                       / (t_r - t_l));

    return higherDegree;
}

ControlPoint CatmullRomCurve::evaluatePoint(const std::vector<float>& knots, float t) const {
    std::size_t start = util::findIndexFromRange(t, knots) - m_degree/2uz - 1uz;

    auto layer = std::ranges::to<std::vector<ControlPoint>>(std::ranges::views::counted(m_controlPoints.begin() + start, m_degree + 1uz));
    auto knotsView = std::ranges::views::counted(knots.begin() + start, m_degree + 1uz);

    while (layer.size() > (getDegree() + 1uz)/2uz)
        layer = nevilleLayer(layer, knotsView, t);

    while (layer.size() > 1uz)
        layer = deboorLayer(layer, knotsView, t);

    return layer.front();
}

std::vector<ControlPoint> CatmullRomCurve::evaluateCurve() const {
    const std::vector<float> knots{generateKnots()};
    std::vector<ControlPoint> smoothCurve;
    smoothCurve.reserve(m_fidelity + 1uz);

    auto bounds = util::Range(knots[getDegree()/2uz], knots[m_controlPoints.size() - getDegree()/2uz - 1uz]);

    for (std::size_t t = 0uz; t < m_fidelity; ++t) {
        const float normalized = static_cast<float>(t)/static_cast<float>(m_fidelity);
        smoothCurve.push_back(evaluatePoint(knots, bounds.lerp(normalized)));
    }
    smoothCurve.push_back(m_controlPoints[m_controlPoints.size() - (getDegree() + 1uz)/2uz]);

    return smoothCurve;
}
