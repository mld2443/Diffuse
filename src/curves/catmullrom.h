#pragma once

#include "curve.h"
#include <map>


class CatmullRomCurve : public SplineCurve, public Interpolant {
public:
    static constexpr const char* name = "catmullrom";

    CatmullRomCurve(std::vector<ControlPoint>&& controlPoints);
    CatmullRomCurve(std::istream& is);

    virtual const char* getName() const override;

protected:
    virtual bool canIncDegree() const override;

    virtual std::vector<ControlPoint> evaluateCurve() const override;

private:
    ControlPoint neville2(std::size_t degree, std::size_t index, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& hash) const;
    ControlPoint deboor(std::size_t d, std::size_t begin, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& memo) const;
    ControlPoint deboor(const std::vector<float>& knots, std::size_t piece, float t) const;

private:
    std::size_t m_nevilleSteps;
};
