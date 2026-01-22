#include "bspline.h"

using namespace std;


BSplineCurve::BSplineCurve(std::vector<ControlPoint>&& controlPoints, std::size_t degree, std::size_t fidelity)
  : Curve(std::move(controlPoints), degree, fidelity, 0.0f)
{}

Curve::CurveType BSplineCurve::getType() const { return Curve::CurveType::bspline; }

std::vector<ControlPoint> BSplineCurve::generateInterpolated() const {
    vector<ControlPoint> interpolated;
    for (uint32_t piece = 0; piece < m_controlPoints.size() - m_degree; piece++)
        for (float t = 0; t < m_fidelity; t++)
            interpolated.push_back(deboor(piece, ((float)t / (float)m_fidelity) + piece + m_degree - 1));

    return interpolated;
}

ControlPoint BSplineCurve::deboor(uint32_t d, uint32_t begin, float t, map<pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    if (d == 0)
        return m_controlPoints[begin];

    auto key = pair<uint32_t, uint32_t>(d, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    return hash[key] = ((deboor(d - 1, begin, t, hash) * (begin + m_degree - t)) +
                        (deboor(d - 1, begin + 1, t, hash) * (t - (d + begin - 1)))) / d;
}

ControlPoint BSplineCurve::deboor(uint32_t piece, float t) const {
    auto hash = map<pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = deboor(m_degree, piece, t, hash);
    return p;
}
