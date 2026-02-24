#pragma once

#include "util/PNGWriter.h"

#include <algorithm> // clamp, fold_left, max, min
#include <cstddef>   // ptrdiff_t, size_t
#include <format>    // format, format_string
#include <ranges>    // from_range, join, transform
#include <utility>   // index_sequence, make_index_sequence
#include <vector>    // vector

#define COPYCONST(T1, ...) std::conditional_t<std::is_const_v<std::remove_reference_t<T1>>, const __VA_ARGS__, __VA_ARGS__>


template <typename T, std::size_t WIDTH, std::size_t HEIGHT>
struct Kernel {
private:
    static constexpr std::ptrdiff_t ORIGINX = WIDTH >> 1uz;
    static constexpr std::ptrdiff_t ORIGINY = HEIGHT >> 1uz;

    template <std::size_t... IDX>
    constexpr Kernel(std::index_sequence<IDX...> &&, T (&&values)[HEIGHT][WIDTH])
      : weights{ values[IDX/WIDTH][IDX%WIDTH]... }
      //, totalWeight(std::ranges::fold_left(std::views::join(weights), T{0}, [](const T& l, const T& r) constexpr { return l + r; })) // not constexpr on clang-21.1.x
    {}

public:
    const T weights[HEIGHT][WIDTH];
    //const T totalWeight;

    constexpr Kernel(T (&&values)[HEIGHT][WIDTH]) : Kernel(std::make_index_sequence<WIDTH*HEIGHT>{}, std::forward<T[HEIGHT][WIDTH]>(values)) {}

    const T& operator[](std::ptrdiff_t x, std::ptrdiff_t y) const { return weights[y][x]; }
};

template <typename T>
struct RGB {
    T r, g, b;

    RGB operator+(this auto&& lhs, const RGB& rhs) { return { lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b }; }
    RGB operator-(this auto&& lhs, const RGB& rhs) { return { lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b }; }
    RGB operator*(this auto&& lhs, const   T& rhs) { return { lhs.r * rhs  , lhs.g * rhs  , lhs.b * rhs   }; }
    RGB operator/(this auto&& lhs, const   T& rhs) { return { lhs.r / rhs  , lhs.g / rhs  , lhs.b / rhs   }; }
    void operator+=(this auto& lhs, const RGB& rhs) { lhs.r += rhs.r; lhs.g += rhs.g; lhs.b += rhs.b; }
    void operator-=(this auto& lhs, const RGB& rhs) { lhs.r -= rhs.r; lhs.g -= rhs.g; lhs.b -= rhs.b; }
    void operator*=(this auto& lhs, const   T& rhs) { lhs.r *= rhs  ; lhs.g *= rhs  ; lhs.b *= rhs  ; }
    void operator/=(this auto& lhs, const   T& rhs) { lhs.r /= rhs  ; lhs.g /= rhs  ; lhs.b /= rhs  ; }
};


template <typename T>
struct RGBA {
    union {
        struct { T r, g, b; };
        RGB<T> rgb = {};
    };
    T a = T{0};

    RGBA operator+(this auto&& lhs, const RGBA& rhs) { return { .rgb = lhs.rgb + rhs.rgb, .a = lhs.a + rhs.a }; }
    RGBA operator-(this auto&& lhs, const RGBA& rhs) { return { .rgb = lhs.rgb - rhs.rgb, .a = lhs.a - rhs.a }; }
    RGBA operator*(this auto&& lhs, const    T& rhs) { return { .rgb = lhs.rgb * rhs    , .a = lhs.a * rhs   }; }
    RGBA operator/(this auto&& lhs, const    T& rhs) { return { .rgb = lhs.rgb / rhs    , .a = lhs.a / rhs   }; }
    void operator+=(this auto& lhs, const RGBA& rhs) { lhs.rgb += rhs.rgb; lhs.a += rhs.a; }
    void operator-=(this auto& lhs, const RGBA& rhs) { lhs.rgb -= rhs.rgb; lhs.a -= rhs.a; }
    void operator*=(this auto& lhs, const    T& rhs) { lhs.rgb *= rhs    ; lhs.a *= rhs  ; }
    void operator/=(this auto& lhs, const    T& rhs) { lhs.rgb /= rhs    ; lhs.a /= rhs  ; }

    RGBA normalize() const { return { .rgb = rgb / a, .a = T{1} }; }

    constexpr auto begin(this auto&& self) -> COPYCONST(decltype(self), T*) { return { &self.r       }; }
    constexpr auto   end(this auto&& self) -> COPYCONST(decltype(self), T*) { return { &self.a + 1uz }; }
};


template <typename T>
struct Image {
    Image(std::size_t width, std::size_t height): m_width(width), m_height(height), m_pixels(height * width) {}
    Image(const Image& other): m_width(other.m_width), m_height(other.m_height), m_pixels(other.m_pixels) {}
    Image(Image&& other): m_width(other.m_width), m_height(other.m_height), m_pixels(std::move(other.m_pixels)) {}

    Image(const std::vector<RGB<T>>& premultiplied, std::size_t width, std::size_t height)
        : m_width(width)
        , m_height(height)
        , m_pixels(std::from_range, std::views::transform(premultiplied, [](const RGB<T>& elem){
            return (elem.r == T{0} && elem.g == T{0} && elem.b == T{0}) ? RGBA<T>{} : RGBA<T>{ .rgb = elem, .a = T{1} };
        }))
    {}

    enum class EdgePolicy {
        BLACK,
        TRANSPARENT,
        CLAMPED,
        WRAPPED,
        MIRROR,
    };

    template <std::size_t WIDTH, std::size_t HEIGHT>
    RGBA<T> sampleKernel(std::ptrdiff_t x_0, std::ptrdiff_t y_0, const Kernel<T, WIDTH, HEIGHT> &kernel, EdgePolicy policy) const {
        RGBA<T> accumulator;

        for (std::ptrdiff_t y_i = 0l; y_i < HEIGHT; ++y_i) {
            for (std::ptrdiff_t x_i = 0l; x_i < WIDTH; ++x_i) {
                const T strength = kernel[x_i, y_i];
                if (strength != T{0}) {
                    const std::ptrdiff_t x = x_0 + x_i - WIDTH/2uz,
                                         y = y_0 + y_i - HEIGHT/2uz;
                    accumulator += samplePoint(x, y, policy) * strength;
                }
            }
        }

        if (accumulator.a > T{0})
            return accumulator.normalize();

        return {};
    }

    Image& operator=(Image&& rhs) = default;

    decltype(auto) operator[](this auto&& self, std::size_t x, std::size_t y) { return self.m_pixels[y*self.m_width + x]; }

    RGBA<T> samplePoint(std::ptrdiff_t x, std::ptrdiff_t y, EdgePolicy policy) const {
        switch (policy) {
            case EdgePolicy::BLACK:
                if (x >= 0l && x < m_width &&
                    y >= 0l && y < m_height)
                    return m_pixels[y*m_width + x];
                else
                    return { 0.f, 0.f, 0.f, 1.f };
            case EdgePolicy::TRANSPARENT:
                if (x >= 0l && x < m_width &&
                    y >= 0l && y < m_height)
                    return m_pixels[y*m_width + x];
                else
                    return { 0.f, 0.f, 0.f, 0.f };
            case EdgePolicy::CLAMPED:
                return m_pixels[std::clamp(y, 0l, static_cast<std::ptrdiff_t>(m_height - 1l))*m_width + std::clamp(x, 0l, static_cast<std::ptrdiff_t>(m_width - 1l))];
            case EdgePolicy::WRAPPED:
                return m_pixels[(y % m_height)*m_width + (x % m_width)];
            case EdgePolicy::MIRROR: //TODO: implement mirror
            default:
                return {};
        }
    }

    std::vector<RGB<T>> convertToRGB() const { return { std::from_range, std::views::transform(m_pixels, [](const RGBA<T>& elem){ return elem.rgb; }) }; }

    void writeTo(std::size_t stepNumber, const char* prefix) const {
        PNGWriter file{std::format("{}-{}_{}x{}.png", stepNumber, prefix, m_width, m_height)};

        std::vector<png_byte> flat_pixels{std::from_range, std::views::transform(std::views::join(m_pixels), [](const float& x){ return static_cast<png_byte>(255.f * x); })};

        file.write(flat_pixels, PixelFormat::RGBA, m_width, m_height);
    }

public:
    std::size_t m_width, m_height;
private:
    std::vector<RGBA<T>> m_pixels;
};


template <typename T>
struct FlipBuffer {
    FlipBuffer(Image<T>&& seed)
      : buffers{ std::move(seed), { seed.m_height, seed.m_width } }
      , flipped(false)
    {}

    void flip() { flipped = !flipped; }

    const Image<T>& src() const { return buffers[flipped ? 1 : 0]; }
          Image<T>& dst()       { return buffers[flipped ? 0 : 1]; }

    std::size_t  width() const { return buffers[0].m_width ; }
    std::size_t height() const { return buffers[0].m_height; }

private:
    Image<T> buffers[2];
    bool flipped;
};


#undef COPYCONST
