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
    ControlPoint deboor(uint32_t d, uint32_t begin, const std::vector<float>& knots, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;
    ControlPoint deboor(const std::vector<float>& knots, uint32_t piece, float t) const;
};
