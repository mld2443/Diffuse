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
    virtual std::vector<ControlPoint> generateInterpolated() const override;

private:
    ControlPoint decasteljau(std::size_t d, std::size_t begin, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const;

    ControlPoint decasteljau(float t) const;
};
