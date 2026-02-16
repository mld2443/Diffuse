#pragma once

#include "curve.h"


class LagrangeCurve : public GlobalCurve, public Parameterized, public Interpolant {
public:
    static constexpr const char* NAME = "lagrange";

    LagrangeCurve(std::vector<ControlPoint>&& controlPoints);
    LagrangeCurve(std::istream& is);

    virtual const char* getName() const override;

protected:
    virtual util::Range<float> getDomain() const override;
};
