#pragma once
#include "curve.h"

#include <list>


class LagrangeCurve : public Curve {
public:
    LagrangeCurve(const std::list<ControlPoint>& cpts, float p = 0.0f, uint32_t f = 50u);

    virtual Curve::CurveType getType() const override;

    virtual void draw(bool drawPoints, bool selected, const ControlPoint* sp) const override;

private:
    ControlPoint neville(const std::vector<float>& knots, float t) const;
};
