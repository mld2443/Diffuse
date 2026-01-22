#pragma once

#include "curve.h"


class BSplineCurve : public Curve {
public:
    BSplineCurve(std::vector<ControlPoint>&& controlPoints);
    using Curve::Curve;

    virtual Curve::CurveType getType() const override;

protected:
    virtual std::vector<ControlPoint> generateInterpolated() const override;

private:
    ControlPoint deboor(uint32_t d, uint32_t begin, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;
    ControlPoint deboor(uint32_t piece, float t) const;
};
