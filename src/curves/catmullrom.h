#pragma once

#include "curve.h"

#include <map>
#include <span>


class CatmullRomCurve : public SplineCurve, public Interpolant {
public:
    static constexpr const char* name = "catmullrom";

    CatmullRomCurve(std::vector<ControlPoint>&& controlPoints);
    CatmullRomCurve(std::istream& is);

    virtual const char* getName() const override;

    virtual void incDegree() override;
    virtual void decDegree() override;


protected:
    virtual std::vector<ControlPoint> evaluateCurve() const override;

private:
    std::vector<ControlPoint> nevilleLayer(const std::span<const ControlPoint>& lowerDegree, const std::span<const float>& knots, float t) const;
    std::vector<ControlPoint> deboorLayer(const std::span<const ControlPoint>& lowerDegree, const std::span<const float>& knots, float t) const;

    ControlPoint evaluatePoint(const std::vector<float>& knots, float t) const;
    ControlPoint evaluatePointImperative(const std::vector<float>& knots, float t) const;

    ControlPoint imperativeNeville(std::size_t degree, std::size_t index, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& hash) const;
    ControlPoint imperativeDeboor(std::size_t d, std::size_t begin, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const;
    ControlPoint deboor(const std::vector<float>& knots, std::size_t piece, float t) const;
};
