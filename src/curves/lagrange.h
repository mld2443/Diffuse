#pragma once
#include "curve.h"


class LagrangeCurve : public Curve {
public:
    LagrangeCurve(std::vector<ControlPoint>&& controlPoints, std::size_t fidelity = 50uz, float parameterization = 0.0f);

    virtual Curve::CurveType getType() const override;

protected:
    virtual std::vector<ControlPoint> generateInterpolated() const override;

private:
    ControlPoint neville(const std::vector<float>& knots, float t) const;
};
