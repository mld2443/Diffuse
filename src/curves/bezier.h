#pragma once

#include "curve.h"

#include <list>


class BezierCurve : public Curve {
public:
    BezierCurve(const std::list<ControlPoint>& cpts, uint32_t f = 50u);

    virtual Curve::CurveType get_type() const override;

    virtual void draw(bool drawPoints, bool selected, const ControlPoint* sp) const override;

    virtual void elevate_degree() override;

private:
    ControlPoint decasteljau(uint32_t d, uint32_t begin, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;

    ControlPoint decasteljau(float t) const;
};
