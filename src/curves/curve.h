#pragma once

#include "../point.h"
#include "../util/common.h"

#include <optional>
#include <span>
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

    void draw(bool drawPoints, bool selected, bool drawSubCurves, bool drawTangents, const std::optional<std::size_t>& selectedLayer, const std::optional<std::size_t>& selectedCurve, const ControlPoint* sp) const;

    // void drawControlPolygon() const;
    // void drawPoints(const ControlPoint* sp) const;
    // void drawTangents() const;
    // void drawCurve() const;
    // void drawSubCurve(const std::optional<std::size_t>& selectedLayer, const std::optional<std::size_t>& selectedCurve) const;

    // provided by parameterization
    virtual util::Range<std::size_t> getDomainIndices() const =0;
    virtual std::vector<float> generateKnots() const =0;

protected:
    struct PyramidEvaluation {
        std::vector<std::vector<f32v2>> layers;
        ControlPoint result;
        f32v2 tangent;
    };

    struct CurveEvaluation {
        std::vector<std::vector<std::vector<f32v2>>> layers;
        std::vector<ControlPoint> results;
        std::vector<f32v2> tangents;
    };

    BaseCurve(std::vector<ControlPoint>&& controlPoints);

    static std::vector<ControlPoint> evaluateLayerForStepWithWindow(const std::span<const ControlPoint>& lowerDegree, float t, const std::span<const float>& knots, const util::Range<std::size_t>& window);

    // provided by locality type
    virtual PyramidEvaluation evaluatePoint(float t, const std::vector<float>& knots) const =0;

    // provided by interpolation type
    virtual std::vector<ControlPoint> evaluateLayerForStep(const std::span<const ControlPoint>& points, float t, const std::span<const float>& knots) const =0;

private:
    CurveEvaluation evaluateCurve() const;

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

    // virtual util::Range<std::size_t> getDomainIndices() const override;
    virtual std::vector<float> generateKnots() const override;

private:
    float m_parameterization;
};


///////////////////////
// MARK: GlobalCurve //
///////////////////////
class GlobalCurve : virtual public BaseCurve {
public:
    virtual std::size_t getDegree() const final override;

protected:
    virtual PyramidEvaluation evaluatePoint(float t, const std::vector<float>& knots) const final override;
};


///////////////////////
// MARK: SplineCurve //
///////////////////////
class SplineCurve : virtual public BaseCurve {
public:
    virtual std::size_t getDegree() const final override;

    bool canIncDegree() const;
    bool canDecDegree() const;

    void incDegree();
    void decDegree();

    using KnotWindows = std::vector<util::Range<std::size_t>>;

    virtual KnotWindows getKnotWindows() const =0;
protected:
    SplineCurve(std::size_t degree, std::size_t increment);
    SplineCurve(std::istream& is, std::size_t increment);

    virtual PyramidEvaluation evaluatePoint(float t, const std::vector<float>& knots) const final override;

private:
    std::size_t m_degree;
    const std::size_t m_increment;
};


///////////////////////
// MARK: Approximant //
///////////////////////
class Approximant : virtual public BaseCurve {
protected:
    virtual std::vector<ControlPoint> evaluateLayerForStep(const std::span<const ControlPoint>& points, float t, const std::span<const float>& knots) const final override;
};


///////////////////////
// MARK: Interpolant //
///////////////////////
class Interpolant : virtual public BaseCurve {
protected:
    virtual std::vector<ControlPoint> evaluateLayerForStep(const std::span<const ControlPoint>& points, float t, const std::span<const float>& knots) const final override;
};
