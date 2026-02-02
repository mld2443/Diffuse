#pragma once

#include "curve.h"


class BezierCurve : public GlobalCurve, public Approximant {
public:
    static constexpr const char* name = "bezier";

    BezierCurve(std::vector<ControlPoint>&& controlPoints);
    BezierCurve(std::istream& is);

    virtual const char* getName() const override;

    void elevateDegree();

protected:
    virtual std::vector<ControlPoint> evaluateCurve() const override;
};
