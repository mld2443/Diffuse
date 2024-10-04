#pragma once

#include "../point.h"

#include <vector>
#include <map>
#include <cstdint>


class Curve {
public:
    enum CurveType { lagrange, bezier, bspline, catmullrom };

    virtual CurveType getType() const =0;
    virtual void draw(bool drawPoints, bool selected, const ControlPoint* sp) const =0;
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

protected:
    std::vector<ControlPoint> m_controlPoints;
    uint32_t m_degree, m_fidelity;
    float m_parameterization;

    static std::vector<float> generateKnots(const std::vector<ControlPoint>& c_points, uint32_t size, float parameterization);

    void draw(const std::vector<ControlPoint>& curve, bool drawPoints, bool selected, const ControlPoint* sp) const;

    ControlPoint neville(uint32_t d, uint32_t begin, const std::vector<float>& knots, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;

    friend std::ostream& operator<<(std::ostream& os, const Curve& c);
};
