#pragma once

#include "../point.h"

#include <vector>


/////////////////////
// MARK: BaseCurve //
/////////////////////
class BaseCurve {
    friend std::ostream& operator<<(std::ostream& os, const BaseCurve& c);

public:
    BaseCurve(std::istream& is);
    virtual ~BaseCurve() = default;

    virtual const char* getName() const =0;

    std::vector<ControlPoint>& getControlPoints();
    std::size_t getFidelity() const;

    // provided by locality type
    virtual std::size_t getDegree() const =0;

    void setFidelity(std::size_t fidelity);

    void draw(bool drawPoints, bool selected, const ControlPoint* sp) const;

protected:
    BaseCurve(std::vector<ControlPoint>&& controlPoints);

    std::vector<ControlPoint> evaluateCurve() const;

    // provided by parameterization
    virtual std::pair<float, float> getDomain() const =0;
    virtual std::vector<float> generateKnots() const =0;

    // provided by locality type
    virtual ControlPoint evaluatePoint(const std::vector<float>& knots, float t) const =0;

    // provided by interpolation type
    virtual std::vector<ControlPoint> evaluateLayer(const std::vector<ControlPoint>& points, const std::vector<float>& knots, float t) const =0;

protected:
    std::vector<ControlPoint> m_controlPoints;

private:
    std::size_t m_fidelity;
};


///////////////////////
// MARK: GlobalCurve //
///////////////////////
class GlobalCurve : virtual public BaseCurve {
public:
    virtual std::size_t getDegree() const override;

protected:
    virtual ControlPoint evaluatePoint(const std::vector<float>& knots, float t) const override;
};


///////////////////////
// MARK: SplineCurve //
///////////////////////
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

    virtual ControlPoint evaluatePoint(const std::vector<float>& knots, float t) const override;

private:
    std::size_t m_degree;
};


/////////////////////////
// MARK: Parameterized //
/////////////////////////
class Parameterized : virtual public BaseCurve {
public:
    float getParam() const;

    void paramInc();
    void paramDec();

protected:
    Parameterized(float parameterization);
    Parameterized(std::istream& is);

    virtual std::pair<float, float> getDomain() const override;
    virtual std::vector<float> generateKnots() const override;

private:
    float m_parameterization;
};


///////////////////////
// MARK: Approximant //
///////////////////////
class Approximant : virtual public BaseCurve {
protected:
    virtual std::vector<ControlPoint> evaluateLayer(const std::vector<ControlPoint>& points, const std::vector<float>& knots, float t) const override;
};


///////////////////////
// MARK: Interpolant //
///////////////////////
class Interpolant : virtual public BaseCurve {
protected:
    virtual std::vector<ControlPoint> evaluateLayer(const std::vector<ControlPoint>& points, const std::vector<float>& knots, float t) const override;
};
