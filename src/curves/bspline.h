#pragma once

#include "curve.h"


class BSplineCurve : public SplineCurve, public Approximant {
public:
    static constexpr const char* name = "bspline";

    BSplineCurve(std::vector<ControlPoint>&& controlPoints);
    BSplineCurve(std::istream& is);

    virtual const char* getName() const override;

protected:
    virtual bool canIncDegree() const override;

    virtual std::vector<ControlPoint> generateInterpolated() const override;

private:
    ControlPoint deboor(uint32_t d, uint32_t begin, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;
    ControlPoint deboor(uint32_t piece, float t) const;
};
