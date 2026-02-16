#pragma once

#include "curve.h"


class BezierCurve : public GlobalCurve, public Approximant {
public:
    static constexpr const char* NAME = "bezier";

    BezierCurve(std::vector<ControlPoint>&& controlPoints);
    BezierCurve(std::istream& is);

    virtual const char* getName() const override;

    void elevateDegree();

protected:
    virtual util::Range<std::size_t> getDomainIndices() const override;
    virtual std::vector<float> generateKnots() const override;
};
