#include "curve.h"

#include <GL/gl.h>
#include <utility>
#include <cmath>
#include <ranges>

using namespace std;


void Curve::elevateDegree() {}

uint32_t Curve::getDegree() const {
    return m_degree;
}

uint32_t Curve::getFidelity() const {
    return m_fidelity;
}

float Curve::getParam() const {
    return m_parameterization;
}

vector<ControlPoint>& Curve::getControlPoints() {
    return m_controlPoints;
}

void Curve::setFidelity(uint32_t f) {
    m_fidelity = f;
}

void Curve::paramInc() {
    if (m_parameterization < 1.5f)
        m_parameterization += 0.5f;
}

void Curve::paramDec() {
    if (m_parameterization > 0.0f)
        m_parameterization -= 0.5f;
}

void Curve::degreeInc() {
    if (getType() == CurveType::bspline && m_degree < m_controlPoints.size() - 1uz)
        m_degree++;
    else if (getType() == CurveType::catmullrom && m_degree < m_controlPoints.size() / 2uz)
        m_degree++;
}

void Curve::degreeDec() {
    if (getType() > Curve::CurveType::bezier && m_degree - 1u)
        m_degree--;
}

Curve::Curve(std::vector<ControlPoint>&& controlPoints, uint32_t degree, uint32_t fidelity, float parameterization)
  : m_controlPoints(std::move(controlPoints))
  , m_degree(degree)
  , m_fidelity(fidelity)
  , m_parameterization(parameterization)
{}


std::vector<float> Curve::generateKnots(float parameterization) const {
    std::vector<float> knots;
    knots.reserve(m_controlPoints.size());

    auto transformAndCumulativeSum = [&](auto& range, float sum = 0.0f){
        return std::views::adjacent_transform<2uz>(range, [&, sum](auto& c0, auto& c1) mutable {
            sum += std::pow((c0.p - c1.p).magnitude(), parameterization);
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

void Curve::draw(bool drawPoints, bool selected, const ControlPoint* sp) const {
    if (m_controlPoints.size() < 2uz)
        return;

    const vector<ControlPoint>& interpolated = generateInterpolated();

    glBegin(GL_QUAD_STRIP); {
//         for (const auto& [c0, c1] : std::views::adjacent<2uz>(generateInterpolated())) {
// 
//         }
        auto i2 = interpolated.begin(), i1 = i2++;
        while (i2 != interpolated.end()) {
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

ControlPoint Curve::neville(uint32_t d, uint32_t begin, const vector<float>& knots, float t, map<pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    if (d == 0)
        return m_controlPoints[begin];

    auto key = pair<uint32_t, uint32_t>(d, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    return hash[key] =
        ((neville(d - 1, begin, knots, t, hash) * (knots[begin + d] - t)) +
         (neville(d - 1, begin + 1, knots, t, hash) * (t - knots[begin]))) /
        (knots[begin + d] - knots[begin]);
}


ostream& operator<<(ostream& os, const Curve& c) {
    os << c.getType() << endl;
    os << c.getDegree() << ' ' << c.getParam() << ' ' << c.getFidelity() << ' ' << c.m_controlPoints.size() << endl;
    for (const auto &pt : c.m_controlPoints)
        os << pt << endl;
    return os;
}
