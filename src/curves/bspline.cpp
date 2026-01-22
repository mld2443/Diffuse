#include "bspline.h"


BSplineCurve::BSplineCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(2uz)
  , Approximant()
{}

BSplineCurve::BSplineCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is)
  , Approximant()
{}

const char* BSplineCurve::getName() const { return name; }

bool BSplineCurve::canIncDegree() const {
    return getDegree() + 1uz < m_controlPoints.size();
}

std::vector<ControlPoint> BSplineCurve::generateInterpolated() const {
    std::vector<ControlPoint> interpolated;
    for (uint32_t piece = 0; piece < m_controlPoints.size() - getDegree(); piece++)
        for (float t = 0; t < m_fidelity; t++)
            interpolated.push_back(deboor(piece, ((float)t / (float)m_fidelity) + piece + getDegree() - 1));

    return interpolated;
}

ControlPoint BSplineCurve::deboor(uint32_t d, uint32_t begin, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const {
    if (d == 0)
        return m_controlPoints[begin];

    auto key = std::pair<uint32_t, uint32_t>(d, begin);
    if (auto hashed = hash.find(key); hashed != hash.end())
        return hashed->second;

    return hash[key] = ((deboor(d - 1, begin, t, hash) * (begin + getDegree() - t)) +
                        (deboor(d - 1, begin + 1, t, hash) * (t - (d + begin - 1)))) / d;
}

ControlPoint BSplineCurve::deboor(uint32_t piece, float t) const {
    auto hash = std::map<std::pair<uint32_t, uint32_t>, ControlPoint>();
    auto p = deboor(getDegree(), piece, t, hash);
    return p;
}
