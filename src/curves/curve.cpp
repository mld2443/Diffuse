#include "curve.h"

#include <GL/gl.h>

#include <cmath>     // pow
#include <ranges>    // adjacent_transform, drop, from_range, zip_transform
#include <utility>   // move


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
            glVertex2fv(&left.coords.x);
            glColor3fv(&i1->r.x);
            glVertex2fv(&right.coords.x);

            ++i1;
            ++i2;
        }

        // Draw last point with special care for tangent.
        auto right = i1->leftside(*(i1 - 1uz));
        auto left = i1->rightside(*(i1 - 1uz));

        glColor3fv(&i1->l.x);
        glVertex2fv(&left.coords.x);
        glColor3fv(&i1->r.x);
        glVertex2fv(&right.coords.x);
    } glEnd();

    if (drawPoints)
        for (const auto &point : m_controlPoints)
            point.draw(isCurveSelected, isCurveSelected && &point == selectedPoint);
}

BaseCurve::BaseCurve(std::vector<ControlPoint>&& controlPoints)
  : m_controlPoints(std::move(controlPoints))
  , m_fidelity(60uz)
{}

std::size_t BaseCurve::getSegmentCount() const {
    return m_controlPoints.size() - getDegree();
}

std::vector<ControlPoint> BaseCurve::evaluateLayerForStepWithWindow(const std::span<const ControlPoint>& lowerDegree, float t, const std::span<const float>& knots, const util::Range<std::size_t>& window) {
    return {std::from_range, std::views::zip_transform([&t](auto& left, auto& right, auto& t_l, auto& t_r) {
        return ((t_r -  t ) * left
              + ( t  - t_l) * right)
              / (t_r - t_l);
    }, lowerDegree, std::views::drop(lowerDegree, 1uz), std::views::drop(knots, window.lower), std::views::drop(knots, window.upper)) };
}

std::vector<ControlPoint> BaseCurve::evaluateCurve() const {
    const std::vector<float> knots = generateKnots();
    std::vector<ControlPoint> smoothCurve;
    smoothCurve.reserve(m_fidelity + 1uz);

    const util::Range indices = getDomainIndices();
    const util::Range domain{knots[indices.lower], knots[indices.upper]};
    const util::Range steps{0uz, m_fidelity};

    for (std::size_t t = steps.lower; t <= steps.upper; ++t)
        smoothCurve.push_back(evaluatePoint(domain.lerp(steps.unmix(t)), knots));

    return smoothCurve;
}


/////////////////////////
// MARK: Parameterized //
/////////////////////////
float Parameterized::getParam() const {
    return m_parameterization;
}

bool Parameterized::canIncParam() const {
    return m_parameterization < 1.5f;
}

bool Parameterized::canDecParam() const {
    return m_parameterization > -1.5f;
}

void Parameterized::paramInc() {
    if (canIncParam())
        m_parameterization += 0.5f;
}

void Parameterized::paramDec() {
    if (canDecParam())
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

    auto transformAndCumulativeSum = [&](auto& range, float sum = 0.0f) {
        return std::views::adjacent_transform<2uz>(range, [&, sum](auto& c0, auto& c1) mutable {
            sum += std::pow((c0.coords - c1.coords).magnitudeSqr(), m_parameterization * 0.5f);
            return sum;
        });
    };

    knots.push_back(0.0f);
    knots.append_range(transformAndCumulativeSum(m_controlPoints));

    // This step is probably optional
    const float normalize = static_cast<float>(knots.size() - 1uz)/knots.back();
    for (auto &knot : knots)
        knot *= normalize;

    return knots;
}


///////////////////////
// MARK: GlobalCurve //
///////////////////////
std::size_t GlobalCurve::getDegree() const {
    return m_controlPoints.size() - 1uz;
}

ControlPoint GlobalCurve::evaluatePoint(float t, const std::vector<float>& knots) const {
    std::vector<ControlPoint> layer(m_controlPoints);

    while (layer.size() > 1uz)
        layer = evaluateLayerForStep(layer, t, knots);

    return layer.front();
}


///////////////////////
// MARK: SplineCurve //
///////////////////////
std::size_t SplineCurve::getDegree() const {
    return m_degree;
}

bool SplineCurve::canIncDegree() const {
    return m_degree + m_increment < m_controlPoints.size();
}

bool SplineCurve::canDecDegree() const {
    return m_degree > m_increment;
}

void SplineCurve::incDegree() {
    if (canIncDegree())
        m_degree += m_increment;
}

void SplineCurve::decDegree() {
    if (canDecDegree())
        m_degree -= m_increment;
}

SplineCurve::SplineCurve(std::size_t degree, std::size_t increment)
  : m_degree(degree)
  , m_increment(increment)
{}

SplineCurve::SplineCurve(std::istream& is, std::size_t increment)
  : m_increment(increment)
{
    is >> m_degree;
}

ControlPoint SplineCurve::evaluatePoint(float t, const std::vector<float>& knots) const {
    const util::Range indices = getDomainIndices();
    const std::size_t interval = util::findIntervalIndex(t, knots);
    const std::size_t segment = indices.clamp(interval) - indices.lower - 1uz;

    std::vector<ControlPoint> workingLayer{m_controlPoints.begin() + segment, m_controlPoints.begin() + segment + m_degree + 1uz};
    const auto knotsView = std::views::drop(knots, segment);

    for (const util::Range<std::size_t>& window : getKnotWindows())
        workingLayer = evaluateLayerForStepWithWindow(workingLayer, t, knotsView, window);

    return workingLayer.front();
}


///////////////////////
// MARK: Approximant //
///////////////////////
std::vector<ControlPoint> Approximant::evaluateLayerForStep(const std::span<const ControlPoint>& lowerDegree, float t, const std::span<const float>&) const {
    // Single layer of De Casteljau's algorithm
    return {std::from_range, std::views::adjacent_transform<2uz>(lowerDegree, [&t](const auto& l, const auto& r){
        return util::Range{ l, r }.lerp(t);
    })};
}


///////////////////////
// MARK: Interpolant //
///////////////////////
std::vector<ControlPoint> Interpolant::evaluateLayerForStep(const std::span<const ControlPoint>& lowerDegree, float t, const std::span<const float>& knots) const {
    // Single layer of Neville's algorithm
    const std::size_t degree = knots.size() - lowerDegree.size() + 1uz;

    return evaluateLayerForStepWithWindow(lowerDegree, t, knots, { 0uz, degree });
}
