#include "curve.h"

#include <GL/gl.h>
#include <utility>
#include <cmath>

using namespace std;


void Curve::elevate_degree() {}

uint32_t Curve::get_degree() const {
    return degree;
}

uint32_t Curve::get_fidelity() const {
    return fidelity;
}

float Curve::get_param() const {
    return parameterization;
}

vector<ControlPoint>& Curve::get_cpts() {
    return c_points;
}

void Curve::set_fidelity(uint32_t f) {
    fidelity = f;
}

void Curve::param_inc() {
    if (parameterization < 1.5)
        parameterization += 0.5;
}

void Curve::param_dec() {
    if (parameterization > 0)
        parameterization -= 0.5;
}

void Curve::degree_inc() {
    if (get_type() == CurveType::bspline && degree < c_points.size() - 1)
        degree++;
    else if (get_type() == CurveType::catmullrom && degree < c_points.size() / 2)
        degree++;
}

void Curve::degree_dec() {
    if (get_type() > Curve::CurveType::bezier && degree - 1)
        degree--;
}

vector<float> Curve::generate_knots(const vector<ControlPoint>& c_points, uint32_t size, float parameterization) {
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

            glColor3iv(&i1->l.x);
            glVertex2fv(&left.p.x);
            glColor3iv(&i1->r.x);
            glVertex2fv(&right.p.x);

            ++i1;
            ++i2;
        }
        auto right = i1->leftside(*(i1 - 1));
        auto left = i1->rightside(*(i1 - 1));

        glColor3iv(&i1->l.x);
        glVertex2fv(&left.p.x);
        glColor3iv(&i1->r.x);
        glVertex2fv(&right.p.x);
    } glEnd();

    if (drawPoints)
        for (auto &p : c_points)
            p.draw(selected * ((&p == sp) + 1));
}

ControlPoint Curve::neville(uint32_t d, uint32_t begin, const vector<float>& knots, float t, map<pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    if (d == 0)
        return c_points[begin];

    auto key = pair<uint32_t, uint32_t>(d, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    return hash[key] =
        ((neville(d - 1, begin, knots, t, hash) * (knots[begin + d] - t)) +
         (neville(d - 1, begin + 1, knots, t, hash) * (t - knots[begin]))) /
        (knots[begin + d] - knots[begin]);
}


ostream& operator<<(ostream& os, const Curve& c) {
    os << c.get_type() << endl;
    os << c.get_degree() << ' ' << c.get_param() << ' ' << c.get_fidelity() << ' ' << c.c_points.size() << endl;
    for (const auto &pt : c.c_points)
        os << pt << endl;
    return os;
}
