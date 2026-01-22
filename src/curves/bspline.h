#pragma once

#include "curve.h"

#include <list>


class BSplineCurve : public Curve {
public:
    BSplineCurve(std::vector<ControlPoint>&& controlPoints, std::size_t degree = 2uz, std::size_t fidelity = 50uz);

    virtual Curve::CurveType getType() const override;

protected:
    virtual std::vector<ControlPoint> generateInterpolated() const override;

private:
    ControlPoint deboor(uint32_t d, uint32_t begin, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;
    ControlPoint deboor(uint32_t piece, float t) const;
};
