#include "bezier.h"


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

    newpts.push_back(m_controlPoints.front());
    for (std::size_t i = 1uz; i < m_controlPoints.size(); ++i)
        newpts.push_back(m_controlPoints[i] * (1 - ((float)i / (float)(getDegree() + 1))) + m_controlPoints[i - 1] * ((float)i / (float)(getDegree() + 1)));
    newpts.push_back(m_controlPoints.back());

    m_controlPoints.clear();
    m_controlPoints = newpts;
}
