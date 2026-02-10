#include <ranges>
#include <algorithm>
#include <utility>
#include <type_traits>


namespace util {
    template <typename T>
    struct Range {
        T lower, upper;

        constexpr inline bool contains(const auto& value) const noexcept {
            if constexpr (requires{ std::is_floating_point_v<T>; })
                return lower <= value && value < upper;
            else
                return std::cmp_less_equal(lower, value) && std::cmp_less(value, upper);
        }
        constexpr inline T clamp(const T& value) const noexcept { return std::clamp<T>(value, lower, upper); }
        constexpr inline T lerp(auto t) const noexcept { return lower + t * (upper - lower); }
    };

    std::size_t findIndexbetweenRanges(auto target, const std::ranges::range auto& sortedRange) {
        auto it = std::ranges::upper_bound(sortedRange, target);
        return static_cast<std::size_t>(std::distance(sortedRange.begin(), it));
    }
}


