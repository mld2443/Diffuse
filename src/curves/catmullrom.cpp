#include "catmullrom.h"

using namespace std;


CatmullRomCurve::CatmullRomCurve(std::vector<ControlPoint>&& controlPoints)
  : Curve(std::move(controlPoints), 2uz)
{}

Curve::CurveType CatmullRomCurve::getType() const { return Curve::CurveType::catmullrom; }

std::vector<ControlPoint> CatmullRomCurve::generateInterpolated() const {
    auto knots = generateKnots(m_parameterization);

    std::vector<ControlPoint> interpolated;
    std::size_t begin = 0uz;
    for (std::size_t piece = m_degree - 1uz; piece < m_controlPoints.size() - m_degree; ++piece, ++begin)
        for (std::size_t t = (knots[piece] * (float)m_fidelity); (float)t < knots[piece + 1uz] * (float)m_fidelity; ++t)
            interpolated.push_back(deboor(knots, begin, (float)t/(float)m_fidelity));
    interpolated.push_back(m_controlPoints[begin + m_degree - 1uz]);

    return interpolated;
}

ControlPoint CatmullRomCurve::deboor(uint32_t d, uint32_t begin, const vector<float>& knots, float t, map<pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    auto key = pair<uint32_t, uint32_t>(d + m_degree, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    if (d == 1u)
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
    return deboor(m_degree, piece, knots, t, hash);
}
