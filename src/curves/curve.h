#pragma once

#include "../point.h"

#include <vector>
#include <map>


class BaseCurve {
    friend std::ostream& operator<<(std::ostream& os, const BaseCurve& c);

public:
    BaseCurve(std::istream& is);
    virtual ~BaseCurve() = default;

    virtual const char* getName() const =0;

    std::vector<ControlPoint>& getControlPoints();
    virtual std::size_t getDegree() const;
    std::size_t getFidelity() const;

    void setFidelity(std::size_t fidelity);

    void draw(bool drawPoints, bool selected, const ControlPoint* sp) const;

protected:
    BaseCurve(std::vector<ControlPoint>&& controlPoints);

    virtual std::vector<ControlPoint> generateInterpolated() const =0;

protected:
    std::vector<ControlPoint> m_controlPoints;
    std::size_t m_fidelity;
};


/////////////////
// GlobalCurve //
/////////////////
class GlobalCurve : virtual public BaseCurve {};


/////////////////
// SplineCurve //
/////////////////
class SplineCurve : virtual public BaseCurve {
public:
    virtual std::size_t getDegree() const override;

    virtual bool canIncDegree() const =0;
    bool canDecDegree() const;

    void incDegree();
    void decDegree();

protected:
    SplineCurve(std::size_t degree);
    SplineCurve(std::istream& is);

private:
    std::size_t m_degree;
};


/////////////////
// Approximant //
/////////////////
class Approximant : virtual public BaseCurve {
protected:
    ControlPoint decasteljau2(const std::vector<ControlPoint>& points, float t) const;
};


/////////////////
// Interpolant //
/////////////////
class Interpolant : virtual public BaseCurve {
public:
    float getParam() const;

    void paramInc();
    void paramDec();

protected:
    Interpolant(float parameterization);
    Interpolant(std::istream& is);

    std::vector<float> generateKnots() const;

    ControlPoint neville(std::size_t degree, std::size_t index, const std::vector<float>& knots, float t, std::map<std::pair<std::size_t, std::size_t>, ControlPoint>& hash) const;
    ControlPoint neville2(const std::vector<ControlPoint>& points, const std::vector<float>& knots, float t) const;


private:
    float m_parameterization;
};
