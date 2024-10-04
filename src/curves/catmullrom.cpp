#include "catmullrom.h"

using namespace std;


CatmullRomCurve::CatmullRomCurve(const list<ControlPoint>& cpts, uint32_t d, float p, uint32_t f) {
    m_controlPoints = vector<ControlPoint>(cpts.begin(), cpts.end());
    m_degree = d;
    m_parameterization = p;
    m_fidelity = f;
}

Curve::CurveType CatmullRomCurve::getType() const { return Curve::CurveType::catmullrom; }

void CatmullRomCurve::draw(bool drawPoints, bool selected, const ControlPoint* sp) const {
    if (m_controlPoints.size() < 2)
        return;

    auto knots = generateKnots(m_controlPoints, (uint32_t)m_controlPoints.size() - 1, m_parameterization);

    vector<ControlPoint> crv;
    int32_t begin = 0;
    for (uint32_t piece = m_degree - 1; piece < m_controlPoints.size() - m_degree; piece++) {
        for (uint32_t t = (knots[piece] * (float)m_fidelity); (float)t < knots[piece + 1] * (float)m_fidelity; t++)
            crv.push_back(deboor(knots, begin, (float)t/(float)m_fidelity));
        begin++;
    }
    crv.push_back(m_controlPoints[begin+m_degree-1]);

    Curve::draw(crv, drawPoints, selected, sp);
}

ControlPoint CatmullRomCurve::deboor(uint32_t d, uint32_t begin, const vector<float>& knots, float t, map<pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    auto key = pair<uint32_t, uint32_t>(d + m_degree, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    if (d == 1)
        return hash[key] =
            ((neville(m_degree - 1, begin, knots, t, hash) * (knots[begin + m_degree] - t)) +
             (neville(m_degree - 1, begin + 1, knots, t, hash) * (t - knots[d + begin - 1]))) /
            (knots[begin + m_degree] - knots[d + begin - 1]);

    return hash[key] =
        ((deboor(d - 1, begin, knots, t, hash) * (knots[begin + m_degree] - t)) +
         (deboor(d - 1, begin + 1, knots, t, hash) * (t - knots[d + begin - 1]))) /
        (knots[begin + m_degree] - knots[d + begin - 1]);
}

ControlPoint CatmullRomCurve::deboor(const vector<float>& knots, uint32_t piece, float t) const {
    auto hash = map<pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = deboor(m_degree, piece, knots, t, hash);
    return p;
}
