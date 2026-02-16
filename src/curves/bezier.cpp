#include "bezier.h"

#include <ranges>


BezierCurve::BezierCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , GlobalCurve()
  , Approximant()
{}

BezierCurve::BezierCurve(std::istream& is)
  : BaseCurve(is)
  , GlobalCurve()
  , Approximant()
{}

const char* BezierCurve::getName() const { return NAME; }

void BezierCurve::elevateDegree() {
    std::vector<ControlPoint> newpts;
    newpts.reserve(m_controlPoints.size() + 1uz);

    const auto normalize = [&](auto i){ return static_cast<float>(i) / static_cast<float>(m_controlPoints.size()); };

    newpts.push_back(m_controlPoints.front());
    for (const auto& [t, left, right] : std::views::zip(std::views::transform(std::views::iota(1uz), normalize),
                                                        m_controlPoints,
                                                        std::views::drop(m_controlPoints, 1uz)))
        newpts.push_back(right + t * (left - right));
    newpts.push_back(m_controlPoints.back());

    m_controlPoints = std::move(newpts);
}

util::Range<std::size_t> BezierCurve::getDomainIndices() const {
    return {0uz, 1uz};
}

std::vector<float> BezierCurve::generateKnots() const {
    return {0.0f, 1.0f};
}
