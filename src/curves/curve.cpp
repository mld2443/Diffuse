#include "curve.h"

#include <GL/gl.h>
#include <utility>
#include <cmath>
#include <ranges>


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

std::size_t BaseCurve::getDegree() const {
    return m_controlPoints.size() - 1uz;
}

std::size_t BaseCurve::getFidelity() const {
    return m_fidelity;
}

void BaseCurve::setFidelity(std::size_t f) {
    m_fidelity = f;
}

void BaseCurve::draw(bool drawPoints, bool selected, const ControlPoint* sp) const {
    if (m_controlPoints.size() < 2uz)
        return;

    const std::vector<ControlPoint>& expanded = generateInterpolated();

    glBegin(GL_QUAD_STRIP); {
        auto i2 = expanded.begin(), i1 = i2++;
        while (i2 != expanded.end()) {
            auto left = i1->leftside(*i2);
            auto right = i1->rightside(*i2);

            glColor3f(i1->l.x, i1->l.y, i1->l.z);
            glVertex2f(left.p.x, left.p.y);
            glColor3f(i1->r.x, i1->r.y, i1->r.z);
            glVertex2f(right.p.x, right.p.y);

            ++i1;
            ++i2;
        }
        auto right = i1->leftside(*(i1 - 1));
        auto left = i1->rightside(*(i1 - 1));

        glColor3f(i1->l.x, i1->l.y, i1->l.z);
        glVertex2f(left.p.x, left.p.y);
        glColor3f(i1->r.x, i1->r.y, i1->r.z);
        glVertex2f(right.p.x, right.p.y);
    } glEnd();

    if (drawPoints)
        for (auto &p : m_controlPoints)
            p.draw(selected * ((&p == sp) + 1));
}

BaseCurve::BaseCurve(std::vector<ControlPoint>&& controlPoints)
  : m_controlPoints(std::move(controlPoints))
  , m_fidelity(50uz)
{}


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

SplineCurve::SplineCurve(std::istream& is) {
    is >> m_degree;
}


///////////////////////
// MARK: Interpolant //
///////////////////////
float Interpolant::getParam() const {
    return m_parameterization;
}

void Interpolant::paramInc() {
    if (m_parameterization < 1.5f)
        m_parameterization += 0.5f;
}

void Interpolant::paramDec() {
    if (m_parameterization > 0.0f)
        m_parameterization -= 0.5f;
}

Interpolant::Interpolant(float parameterization)
  : m_parameterization(parameterization)
{}

Interpolant::Interpolant(std::istream& is) {
    is >> m_parameterization;
}

std::vector<float> Interpolant::generateKnots() const {
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

ControlPoint Interpolant::neville(std::size_t d, std::size_t begin, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const {
    if (d == 0)
        return m_controlPoints[begin];

    auto key = std::pair(d, begin);
    if (auto hashed = memo.find(key); hashed != memo.end())
        return hashed->second;

    return memo[key] =
        ((neville(d - 1, begin, knots, t, memo) * (knots[begin + d] - t)) +
         (neville(d - 1, begin + 1, knots, t, memo) * (t - knots[begin]))) /
        (knots[begin + d] - knots[begin]);
}


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
    if (const Interpolant* interp = dynamic_cast<const Interpolant*>(&curve))
        os << ' ' << interp->getParam();

    return os << std::endl;
}
