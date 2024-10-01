#include "catmullrom.h"

using namespace std;


CatmullRomCurve::CatmullRomCurve(const list<ControlPoint>& cpts, uint32_t d, float p, uint32_t f) {
    c_points = vector<ControlPoint>(cpts.begin(), cpts.end());
    degree = d;
    parameterization = p;
    fidelity = f;
}

Curve::CurveType CatmullRomCurve::get_type() const { return Curve::CurveType::catmullrom; }

void CatmullRomCurve::draw(bool drawPoints, bool selected, const ControlPoint* sp) const {
    if (c_points.size() < 2)
        return;

    auto knots = generate_knots(c_points, (uint32_t)c_points.size() - 1, parameterization);

    vector<ControlPoint> crv;
    int32_t begin = 0;
    for (uint32_t piece = degree - 1; piece < c_points.size() - degree; piece++) {
        for (uint32_t t = (knots[piece] * (float)fidelity); (float)t < knots[piece + 1] * (float)fidelity; t++)
            crv.push_back(deboor(knots, begin, (float)t/(float)fidelity));
        begin++;
    }
    crv.push_back(c_points[begin+degree-1]);

    Curve::draw(crv, drawPoints, selected, sp);
}

ControlPoint CatmullRomCurve::deboor(uint32_t d, uint32_t begin, const vector<float>& knots, float t, map<pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    auto key = pair<uint32_t, uint32_t>(d + degree, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    if (d == 1)
        return hash[key] =
            ((neville(degree - 1, begin, knots, t, hash) * (knots[begin + degree] - t)) +
             (neville(degree - 1, begin + 1, knots, t, hash) * (t - knots[d + begin - 1]))) /
            (knots[begin + degree] - knots[d + begin - 1]);

    return hash[key] =
        ((deboor(d - 1, begin, knots, t, hash) * (knots[begin + degree] - t)) +
         (deboor(d - 1, begin + 1, knots, t, hash) * (t - knots[d + begin - 1]))) /
        (knots[begin + degree] - knots[d + begin - 1]);
}

ControlPoint CatmullRomCurve::deboor(const vector<float>& knots, uint32_t piece, float t) const {
    auto hash = map<pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = deboor(degree, piece, knots, t, hash);
    return p;
}
