#pragma once

#include "../point.h"

#include <vector>
#include <map>
#include <cstdint>


class Curve {
public:
    enum CurveType { lagrange, bezier, bspline, catmullrom };

    virtual ~Curve() = default;

    virtual CurveType getType() const =0;

    void draw(bool drawPoints, bool selected, const ControlPoint* sp) const;
    virtual void elevateDegree();

    uint32_t getDegree() const;
    uint32_t getFidelity() const;
    float getParam() const;
    std::vector<ControlPoint>& getControlPoints();

    void setFidelity(uint32_t f);

    void paramInc();
    void paramDec();

    void degreeInc();
    void degreeDec();

    friend std::ostream& operator<<(std::ostream& os, const Curve& c);

protected:
    Curve(std::vector<ControlPoint>&&, uint32_t, uint32_t, float);

    std::vector<float> generateKnots(float parameterization) const;

    virtual std::vector<ControlPoint> generateInterpolated() const =0;

    ControlPoint neville(uint32_t d, uint32_t begin, const std::vector<float>& knots, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;

    std::vector<ControlPoint> m_controlPoints;
    std::size_t m_degree, m_fidelity;
    float m_parameterization;
};
