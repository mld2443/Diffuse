#pragma once

#include "curve.h"


class BSplineCurve : public SplineCurve, public Parameterized, public Approximant {
public:
    static constexpr const char* NAME = "bspline";

    BSplineCurve(std::vector<ControlPoint>&& controlPoints);
    BSplineCurve(std::istream& is);

    virtual const char* getName() const override;

protected:
    virtual util::Range<float> getDomain() const override;
    virtual std::vector<float> generateKnots() const override;
};
