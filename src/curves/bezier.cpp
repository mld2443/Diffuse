#include "bezier.h"

#include <ranges> // drop, iota, zip_transform


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

    newpts.push_back(m_controlPoints.front());
    newpts.append_range(std::views::zip_transform([&](std::size_t i, const ControlPoint& left, const ControlPoint& right) {
        const float t = static_cast<float>(i) / static_cast<float>(m_controlPoints.size());
        return right + t * (left - right);
    }, std::views::iota(1uz), m_controlPoints, std::views::drop(m_controlPoints, 1uz)));
    newpts.push_back(m_controlPoints.back());

    m_controlPoints = std::move(newpts);
}

util::Range<std::size_t> BezierCurve::getDomainIndices() const {
    return {0uz, 1uz};
}

std::vector<float> BezierCurve::generateKnots() const {
    return {0.0f, 1.0f};
}
