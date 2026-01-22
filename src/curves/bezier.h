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
    ControlPoint decasteljau(uint32_t d, uint32_t begin, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;

    ControlPoint decasteljau(float t) const;
};
