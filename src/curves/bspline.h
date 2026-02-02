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

    virtual std::vector<ControlPoint> evaluateCurve() const override;

private:
    ControlPoint deboor(std::size_t d, std::size_t begin, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const;
    ControlPoint deboor(std::size_t piece, float t) const;
};
