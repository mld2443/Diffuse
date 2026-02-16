#pragma once

#include "../point.h"
#include "../util/common.h"

#include <vector>
#include <span>


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

    std::size_t getSegmentCount() const;

    // provided by parameterization
    virtual util::Range<float> getDomain() const =0;
    virtual std::vector<float> generateKnots() const =0;

    // provided by locality type
    virtual ControlPoint evaluatePoint(const std::vector<float>& knots, float t) const =0;

    // provided by interpolation type
    virtual std::vector<ControlPoint> evaluateLayer(const std::span<ControlPoint>& points, const std::span<const float>& knots, float t) const =0;

private:
    std::vector<ControlPoint> evaluateCurve() const;

protected:
    std::vector<ControlPoint> m_controlPoints;

private:
    std::size_t m_fidelity;
};


/////////////////////////
// MARK: Parameterized //
/////////////////////////
class Parameterized : virtual public BaseCurve {
public:
    float getParam() const;

    bool canIncParam() const;
    bool canDecParam() const;

    void paramInc();
    void paramDec();

protected:
    Parameterized(float parameterization);
    Parameterized(std::istream& is);

    // virtual util::Range<float> getDomain() const override;
    virtual std::vector<float> generateKnots() const override;

private:
    float m_parameterization;
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

    bool canIncDegree() const;
    bool canDecDegree() const;

    void incDegree();
    void decDegree();

protected:
    SplineCurve(std::size_t degree, std::size_t increment);
    SplineCurve(std::istream& is, std::size_t increment);

    virtual ControlPoint evaluatePoint(const std::vector<float>& knots, float t) const override;

private:
    std::size_t m_degree;
    const std::size_t m_increment;
};


///////////////////////
// MARK: Approximant //
///////////////////////
class Approximant : virtual public BaseCurve {
protected:
    virtual std::vector<ControlPoint> evaluateLayer(const std::span<ControlPoint>& points, const std::span<const float>& knots, float t) const override;
};


///////////////////////
// MARK: Interpolant //
///////////////////////
class Interpolant : virtual public BaseCurve {
protected:
    virtual std::vector<ControlPoint> evaluateLayer(const std::span<ControlPoint>& points, const std::span<const float>& knots, float t) const override;
};
