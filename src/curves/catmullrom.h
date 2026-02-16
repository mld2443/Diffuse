#pragma once

#include "curve.h"

#include <span>


class CatmullRomCurve : public SplineCurve, public Parameterized, public Interpolant {
public:
    static constexpr const char* NAME = "catmullrom";

    CatmullRomCurve(std::vector<ControlPoint>&& controlPoints);
    CatmullRomCurve(std::istream& is);

    virtual const char* getName() const override;

protected:
    virtual util::Range<float> getDomain() const override;
    virtual std::vector<float> generateKnots() const override;

private:
    std::size_t nevilleDegree() const;
    std::size_t deboorDegree() const;

    std::vector<ControlPoint> nevilleLayer(const std::span<const ControlPoint>& lowerDegree, const std::span<const float>& knots, float t) const;
    std::vector<ControlPoint> deboorLayer(const std::span<const ControlPoint>& lowerDegree, const std::span<const float>& knots, float t) const;
};
