#include "curve.h"

#include <GL/gl.h>
#include <utility>
#include <cmath>
#include <ranges>


////////////////////////////
// MARK: Global Functions //
////////////////////////////
std::ostream& operator<<(std::ostream& os, const BaseCurve& curve) {
    os << curve.getName() << ' ' << curve.m_controlPoints.size() << std::endl;
    for (const auto &point : curve.m_controlPoints)
        os << point << std::endl;
    os << curve.getFidelity();

    if (const SplineCurve* spline = dynamic_cast<const SplineCurve*>(&curve))
        os << ' ' << spline->getDegree();
    if (const Parameterized* params = dynamic_cast<const Parameterized*>(&curve))
        os << ' ' << params->getParam();

    return os << std::endl;
}


/////////////////////
// MARK: BaseCurve //
/////////////////////
BaseCurve::BaseCurve(std::istream& is) {
    std::size_t count;
    is >> count;

    m_controlPoints.reserve(count);
    for (std::size_t pointIndex = 0uz; pointIndex < count; ++pointIndex) {
        ControlPoint p;
        is >> p;
        m_controlPoints.push_back(p);
    }

    is >> m_fidelity;
}

std::vector<ControlPoint>& BaseCurve::getControlPoints() {
    return m_controlPoints;
}

std::size_t BaseCurve::getFidelity() const {
    return m_fidelity;
}

void BaseCurve::setFidelity(std::size_t f) {
    m_fidelity = f;
}

void BaseCurve::draw(bool drawPoints, bool isCurveSelected, const ControlPoint* selectedPoint) const {
    if (m_controlPoints.size() < 2uz)
        return;

    auto sampled = evaluateCurve();

    glBegin(GL_QUAD_STRIP); {
        auto i2 = sampled.begin(), i1 = i2++;
        while (i2 != sampled.end()) {
            auto left = i1->leftside(*i2);
            auto right = i1->rightside(*i2);

            glColor3fv(&i1->l.x);
            glVertex2fv(&left.p.x);
            glColor3fv(&i1->r.x);
            glVertex2fv(&right.p.x);

            ++i1;
            ++i2;
        }

        // Draw last point with special care for tangent.
        auto right = i1->leftside(*(i1 - 1uz));
        auto left = i1->rightside(*(i1 - 1uz));

        glColor3fv(&i1->l.x);
        glVertex2fv(&left.p.x);
        glColor3fv(&i1->r.x);
        glVertex2fv(&right.p.x);
    } glEnd();

    if (drawPoints)
        for (const auto &point : m_controlPoints)
            point.draw(isCurveSelected, isCurveSelected && &point == selectedPoint);
}

BaseCurve::BaseCurve(std::vector<ControlPoint>&& controlPoints)
  : m_controlPoints(std::move(controlPoints))
  , m_fidelity(50uz)
{}

std::vector<ControlPoint> BaseCurve::evaluateCurve() const {
    const std::vector<float> knots = generateKnots();
    std::vector<ControlPoint> smoothCurve;
    smoothCurve.reserve(m_fidelity + 1uz);

    const float range = static_cast<float>(m_fidelity);

    for (float t = 0uz; t <= range; t += 1.0f)
        smoothCurve.push_back(evaluatePoint(knots, t));

    return smoothCurve;
}


///////////////////////
// MARK: GlobalCurve //
///////////////////////
std::size_t GlobalCurve::getDegree() const {
    return m_controlPoints.size() - 1uz;
}

ControlPoint GlobalCurve::evaluatePoint(const std::vector<float>& knots, float t) const {
    std::vector<ControlPoint> layer(m_controlPoints);

    while (layer.size() > 1uz)
        layer = evaluateLayer(layer, knots, t);

    return layer.front();
}


///////////////////////
// MARK: SplineCurve //
///////////////////////
std::size_t SplineCurve::getDegree() const {
    return m_degree;
}

bool SplineCurve::canDecDegree() const {
    return m_degree > 1uz;
}

void SplineCurve::incDegree() {
    if (canIncDegree())
        ++m_degree;
}

void SplineCurve::decDegree() {
    if (canDecDegree())
        --m_degree;
}

SplineCurve::SplineCurve(std::size_t degree)
  : m_degree(degree)
{}

SplineCurve::SplineCurve(std::istream& is)
  : BaseCurve(is)
{
    is >> m_degree;
}

ControlPoint SplineCurve::evaluatePoint(const std::vector<float>& knots, float t) const {
    return {};
}


/////////////////////////
// MARK: Parameterized //
/////////////////////////
float Parameterized::getParam() const {
    return m_parameterization;
}

void Parameterized::paramInc() {
    if (m_parameterization < 1.5f)
        m_parameterization += 0.5f;
}

void Parameterized::paramDec() {
    if (m_parameterization > 0.0f)
        m_parameterization -= 0.5f;
}

Parameterized::Parameterized(float parameterization)
  : m_parameterization(parameterization)
{}

Parameterized::Parameterized(std::istream& is) {
    is >> m_parameterization;
}

std::vector<float> Parameterized::generateKnots() const {
    std::vector<float> knots;
    knots.reserve(m_controlPoints.size());

    auto transformAndCumulativeSum = [&](auto& range, float sum = 0.0f){
        return std::views::adjacent_transform<2uz>(range, [&, sum](auto& c0, auto& c1) mutable {
            sum += std::pow((c0.p - c1.p).magnitude(), m_parameterization);
            return sum;
        });
    };

    knots.push_back(0.0f);
    for (const auto& knot : transformAndCumulativeSum(m_controlPoints))
        knots.push_back(knot);

    const float normalize = static_cast<float>(knots.size() - 1uz)/knots.back();
    for (auto &knot : knots)
        knot *= normalize;

    return knots;
}


///////////////////////
// MARK: Approximant //
///////////////////////
std::vector<ControlPoint> Approximant::evaluateLayer(const std::vector<ControlPoint>& lowerDegree, const std::vector<float>&, float t) const {
    // De Casteljau's algorithm
    std::vector<ControlPoint> higherDegree;
    higherDegree.reserve(lowerDegree.size() - 1uz);

    using namespace std::ranges::views;
    for (const auto& [left, right] : adjacent<2uz>(lowerDegree))
        higherDegree.push_back((1.0f - t) * left + t * right);

    return higherDegree;
}


///////////////////////
// MARK: Interpolant //
///////////////////////
std::vector<ControlPoint> Interpolant::evaluateLayer(const std::vector<ControlPoint>& lowerDegree, const std::vector<float>& knots, float t) const {
    // Neville's algorithm
    const std::size_t degree = knots.size() - lowerDegree.size() + 1uz;

    std::vector<ControlPoint> higherDegree;
    higherDegree.reserve(lowerDegree.size() - 1uz);

    for (const auto& [leftPoint, rightPoint, leftKnot, rightKnot] :
            std::ranges::views::zip(lowerDegree, std::ranges::views::drop(lowerDegree, 1uz), knots, std::ranges::views::drop(knots, degree)))
        higherDegree.push_back(((rightKnot - t) * leftPoint + (t - leftKnot) * rightPoint)
                             / (rightKnot - leftKnot));

    return higherDegree;
}
