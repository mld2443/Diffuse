#pragma once

#include "curve.h"

#include <list>


class CatmullRomCurve : public Curve {
public:
    CatmullRomCurve(const std::list<ControlPoint>& cpts, uint32_t d = 2u, float p = 0.0f, uint32_t f = 50u);

    virtual Curve::CurveType getType() const override;

    virtual void draw(bool drawPoints, bool selected, const ControlPoint* sp) const override;

private:
    ControlPoint deboor(uint32_t d, uint32_t begin, const std::vector<float>& knots, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;
    ControlPoint deboor(const std::vector<float>& knots, uint32_t piece, float t) const;
};
