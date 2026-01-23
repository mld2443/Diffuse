#pragma once

#include "curve.h"


class LagrangeCurve : public GlobalCurve, public Interpolant {
public:
    static constexpr const char* name = "lagrange";

    LagrangeCurve(std::vector<ControlPoint>&& controlPoints);
    LagrangeCurve(std::istream& is);

    virtual const char* getName() const override;

protected:
    virtual std::vector<ControlPoint> generateInterpolated() const override;
};
