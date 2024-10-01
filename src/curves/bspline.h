#pragma once

#include "curve.h"

#include <list>


class BSplineCurve : public Curve {
public:
    BSplineCurve(const std::list<ControlPoint>& cpts, uint32_t d = 2, uint32_t f = 50);

    virtual Curve::CurveType get_type() const override;

    virtual void draw(bool drawPoints, bool selected, const ControlPoint* sp) const override;

private:
    ControlPoint deboor(uint32_t d, uint32_t begin, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;
    ControlPoint deboor(uint32_t piece, float t) const;
};
