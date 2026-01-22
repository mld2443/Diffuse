#pragma once

#include "curve.h"


class CatmullRomCurve : public SplineCurve, public Interpolant {
public:
    static constexpr const char* name = "catmullrom";

    CatmullRomCurve(std::vector<ControlPoint>&& controlPoints);
    CatmullRomCurve(std::istream& is);

    virtual const char* getName() const override;

protected:
    virtual bool canIncDegree() const override;

    virtual std::vector<ControlPoint> generateInterpolated() const override;

private:
    ControlPoint deboor(std::size_t d, std::size_t begin, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const;
    ControlPoint deboor(const std::vector<float>& knots, std::size_t piece, float t) const;
};
