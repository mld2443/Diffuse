#pragma once

#include <algorithm> // fold_left, max, min
#include <cstddef>   // ptrdiff_t, size_t
#include <ranges>    // from_range, join, transform
#include <utility>   // index_sequence, make_index_sequence
#include <vector>    // vector


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

    template <std::size_t WIDTH, std::size_t HEIGHT>
    RGB<T> sample(std::ptrdiff_t x_0, std::ptrdiff_t y_0, const Kernel<T, WIDTH, HEIGHT> &kernel) const {
        RGBA<T> accumulator;

        for (std::ptrdiff_t y_i = 0l; y_i < HEIGHT; ++y_i) {
            for (std::ptrdiff_t x_i = 0l; x_i < WIDTH; ++x_i) {
                const T strength = kernel[x_i, y_i];
                if (strength != T{0}) {
                    const std::ptrdiff_t x = x_0 + x_i - WIDTH/2uz,
                                         y = y_0 + y_i - HEIGHT/2uz;
                    if (x >= 0l && x < m_width &&
                        y >= 0l && y < m_height) {
                        const RGBA sample{ .rgb = this->operator[](x, y).rgb, .a = T{1} };
                        accumulator += sample * strength;
                    }
                }
            }
        }

        return accumulator.normalize().rgb;
    }

    Image& operator=(Image&& rhs) = default;

    decltype(auto) operator[](this auto&& self, std::size_t x, std::size_t y) { return self.m_pixels[y*self.m_width + x]; }
    RGBA<T>* getPtr() { return m_pixels.data(); }

    std::vector<RGB<T>> convertToRGB() const { return { std::from_range, std::views::transform(m_pixels, [](const RGBA<T>& elem){ return elem.rgb; }) }; }

public:
    const std::size_t m_width, m_height;
private:
    std::vector<RGBA<T>> m_pixels;
};
