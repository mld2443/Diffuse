#include "curve.h"

#include <GL/gl.h>
#include <utility>
#include <cmath>

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
    if (m_parameterization < 1.5)
        m_parameterization += 0.5;
}

void Curve::paramDec() {
    if (m_parameterization > 0)
        m_parameterization -= 0.5;
}

void Curve::degreeInc() {
    if (getType() == CurveType::bspline && m_degree < m_controlPoints.size() - 1)
        m_degree++;
    else if (getType() == CurveType::catmullrom && m_degree < m_controlPoints.size() / 2)
        m_degree++;
}

void Curve::degreeDec() {
    if (getType() > Curve::CurveType::bezier && m_degree - 1)
        m_degree--;
}

vector<float> Curve::generateKnots(const vector<ControlPoint>& c_points, uint32_t size, float parameterization) {
    vector<float> knots;
    knots.push_back(0.0);
    for (uint32_t i = 0; i < size; i++)
        knots.push_back(knots.back() + pow((c_points[i].p - c_points[i + 1].p).magnitude(), parameterization));
    for (auto &knot : knots)
        knot = (knot/knots.back()) * (knots.size() - 1);
    return knots;
}

void Curve::draw(const vector<ControlPoint>& curve, bool drawPoints, bool selected, const ControlPoint* sp) const {
    glBegin(GL_QUAD_STRIP); {
        auto i2 = curve.begin(), i1 = i2++;
        while (i2 != curve.end()) {
            auto left = i1->leftside(*i2);
            auto right = i1->rightside(*i2);

            glColor3f(i1->l.x, i1->l.y, i1->l.z);
            glVertex2f(left.p.x, left.p.y);
            glColor3f(i1->r.x, i1->r.y, i1->r.z);
            glVertex2f(right.p.x, left.p.y);

            ++i1;
            ++i2;
        }
        auto right = i1->leftside(*(i1 - 1));
        auto left = i1->rightside(*(i1 - 1));

        glColor3f(i1->l.x, i1->l.y, i1->l.z);
        glVertex2f(left.p.x, left.p.y);
        glColor3f(i1->r.x, i1->r.y, i1->r.z);
        glVertex2f(right.p.x, left.p.y);
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
