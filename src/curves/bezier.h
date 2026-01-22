#pragma once

#include "curve.h"


class BezierCurve : public Curve {
public:
    BezierCurve(std::vector<ControlPoint>&& controlPoints);
    using Curve::Curve;

    virtual Curve::CurveType getType() const override;

    virtual void elevateDegree() override;

protected:
    virtual std::vector<ControlPoint> generateInterpolated() const override;

private:
    ControlPoint decasteljau(uint32_t d, uint32_t begin, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;

    ControlPoint decasteljau(float t) const;
};
