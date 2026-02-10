#pragma once

#include "curve.h"

#include <span>


class BSplineCurve : public SplineCurve, public Approximant {
public:
    static constexpr const char* name = "bspline";

    BSplineCurve(std::vector<ControlPoint>&& controlPoints);
    BSplineCurve(std::istream& is);

    virtual const char* getName() const override;

protected:
    virtual std::vector<ControlPoint> evaluateCurve() const override;

private:
    std::vector<ControlPoint> deboorLayer(const std::span<const ControlPoint>& lowerDegree, const std::span<const float>& knots, float t) const;

    ControlPoint evaluatePoint(const std::span<const float>& knots, float t) const;
};
