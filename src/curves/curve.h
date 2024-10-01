#pragma once

#include "../point.h"

#include <vector>
#include <map>
#include <cstdint>


class Curve {
public:
    enum CurveType { lagrange, bezier, bspline, catmullrom };

    virtual CurveType get_type() const =0;
    virtual void draw(bool drawPoints, bool selected, const ControlPoint* sp) const =0;
    virtual void elevate_degree();

    uint32_t get_degree() const;
    uint32_t get_fidelity() const;
    float get_param() const;
    std::vector<ControlPoint>& get_cpts();

    void set_fidelity(uint32_t f);

    void param_inc();
    void param_dec();

    void degree_inc();
    void degree_dec();

protected:
    std::vector<ControlPoint> c_points;
    uint32_t degree, fidelity;
    float parameterization;

    static std::vector<float> generate_knots(const std::vector<ControlPoint>& c_points, uint32_t size, float parameterization);

    void draw(const std::vector<ControlPoint>& curve, bool drawPoints, bool selected, const ControlPoint* sp) const;

    ControlPoint neville(uint32_t d, uint32_t begin, const std::vector<float>& knots, float t, std::map<std::pair<uint32_t, uint32_t>, ControlPoint>& hash) const;

    friend std::ostream& operator<<(std::ostream& os, const Curve& c);
};
