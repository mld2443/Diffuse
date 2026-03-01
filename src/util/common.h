#pragma once

#include <algorithm>   // clamp, upper_bound
#include <format>      // format, format_string
#include <iterator>    // distance
#include <ranges>      // ranges::range
#include <string>      // string
#include <type_traits> // is_floating_point_v
#include <utility>     // cmp_less, cmp_less_equal, forward


// MARK: Build Flags
#if NDEBUG
#define DEBUG 0
#define RELEASE 1
#define DEBUGONLY(...)
#else
#define DEBUG 1
#define RELEASE 0
#define DEBUGONLY(...) __VA_ARGS__
#endif

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
        constexpr inline double unmix(const T& value) const noexcept { return static_cast<double>(value - lower)/static_cast<double>(upper - lower); }
    };

    template <typename T, std::size_t N>
    consteval std::size_t arrlen(const T (&)[N]) { return N; }

    template <std::size_t N>
    consteval std::size_t strlen(const char (&str)[N]) { return str[N - 1uz] == '\0' ? N - 1uz : N; }

    std::size_t findIntervalIndex(auto target, const std::ranges::range auto& sortedRange) {
        auto it = std::ranges::upper_bound(sortedRange, target);
        return static_cast<std::size_t>(std::distance(sortedRange.begin(), it));
    }

    // MARK: Error Report
    struct Critical {
        template <typename... Args>
        Critical(const std::format_string<Args...>& fmt, Args&&... args) : report(std::format(fmt, std::forward<Args>(args)...)) {}

        const std::string report;
    };
}

template <typename T, typename CharT>
struct std::formatter<util::Range<T>, CharT>
{
    // Formatter for the underlying T
    std::formatter<T, CharT> value_fmt;

    constexpr auto parse(std::basic_format_parse_context<CharT>& ctx) {
        // Delegate parsing of format specifiers to T's formatter
        return value_fmt.parse(ctx);
    }

    template <typename FormatContext>
    auto format(const util::Range<T>& r, FormatContext& ctx) const {
        auto out = ctx.out();

        *out++ = CharT{'['};
        out = value_fmt.format(r.lower, ctx);
        *out++ = CharT{','};
        *out++ = CharT{' '};
        out = value_fmt.format(r.upper, ctx);
        *out++ = CharT{')'};

        return out;
    }
};
