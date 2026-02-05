#pragma once

#include "curve.h"


class CatmullRomCurve : public SplineCurve, public Parameterized, public Interpolant {
public:
    static constexpr const char* NAME = "catmullrom";

    CatmullRomCurve(std::vector<ControlPoint>&& controlPoints);
    CatmullRomCurve(std::istream& is);

    virtual const char* getName() const override;

protected:
    virtual bool canIncDegree() const override;

private:
    std::size_t m_nevilleSteps;
};
