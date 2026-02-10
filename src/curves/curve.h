#pragma once

#include "../point.h"

#include <vector>


class BaseCurve {
    friend std::ostream& operator<<(std::ostream& os, const BaseCurve& c);

public:
    BaseCurve(std::istream& is);
    virtual ~BaseCurve() = default;

    virtual const char* getName() const =0;

    std::vector<ControlPoint>& getControlPoints();
    virtual std::size_t getDegree() const =0;
    std::size_t getFidelity() const;

    void setFidelity(std::size_t fidelity);

    void draw(bool drawPoints, bool selected, const ControlPoint* sp) const;

protected:
    BaseCurve(std::vector<ControlPoint>&& controlPoints);

    std::size_t getSegmentCount() const;

    virtual std::vector<ControlPoint> evaluateCurve() const =0;

protected:
    std::vector<ControlPoint> m_controlPoints;
    std::size_t m_fidelity;
};


/////////////////
// GlobalCurve //
/////////////////
class GlobalCurve : virtual public BaseCurve {
protected:
    virtual std::size_t getDegree() const override;
};


/////////////////
// SplineCurve //
/////////////////
class SplineCurve : virtual public BaseCurve {
public:
    virtual std::size_t getDegree() const override;

    bool canIncDegree() const;
    bool canDecDegree() const;

    void incDegree();
    void decDegree();

protected:
    SplineCurve(std::size_t degree, std::size_t increment);
    SplineCurve(std::istream& is, std::size_t increment);

private:
    std::size_t m_degree;
    const std::size_t m_increment;
};


/////////////////
// Approximant //
/////////////////
class Approximant : virtual public BaseCurve {
protected:
    ControlPoint decasteljau(const std::vector<ControlPoint>& points, float t) const;
};


/////////////////
// Interpolant //
/////////////////
class Interpolant : virtual public BaseCurve {
public:
    float getParam() const;

    bool canIncParam() const;
    bool canDecParam() const;

    void paramInc();
    void paramDec();

protected:
    Interpolant(float parameterization);
    Interpolant(std::istream& is);

    std::vector<float> generateKnots() const;

    ControlPoint neville(const std::vector<ControlPoint>& points, const std::vector<float>& knots, float t) const;


private:
    float m_parameterization;
};
