#include "catmullrom.h"

#include <ranges> // iota, transform


CatmullRomCurve::CatmullRomCurve(std::vector<ControlPoint>&& controlPoints)
  : BaseCurve(std::move(controlPoints))
  , SplineCurve(3uz, 2uz)
  , Parameterized(0.0f)
  , Interpolant()
{}

CatmullRomCurve::CatmullRomCurve(std::istream& is)
  : BaseCurve(is)
  , SplineCurve(is, 2uz)
  , Parameterized(is)
  , Interpolant()
{}

const char* CatmullRomCurve::getName() const { return NAME; }

util::Range<std::size_t> CatmullRomCurve::getDomainIndices() const {
    const std::size_t degree = getDegree();
    return { degree >> 1uz, m_controlPoints.size() - ((degree + 1uz) >> 1uz) };
}

SplineCurve::KnotWindows CatmullRomCurve::getKnotWindows() const {
    const std::size_t degree = getDegree();
    const std::size_t nevilleSteps = (degree + 1uz) >> 1uz;
    const std::size_t deboorSteps = degree >> 1uz;

    KnotWindows bounds;
    bounds.reserve(degree);

    bounds.append_range(std::views::transform(std::views::iota(0uz, nevilleSteps), [](auto step) { return util::Range{        0uz, step + 1uz   }; }));
    bounds.append_range(std::views::transform(std::views::iota(0uz, deboorSteps), [&](auto step) { return util::Range{ step + 1uz, nevilleSteps }; }));

    return bounds;
}
