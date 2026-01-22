#pragma once

#include <cstddef>
#include <ranges>
#include <vector>


template <typename T, std::size_t N>
struct Kernel {
    T weights[N][N];

    Kernel(T (&&values)[N][N]): weights(values) {}
};

template <typename T>
struct RGB {
    T r, g, b;

    RGB operator+(const RGB& rhs) const { return { r+rhs.r, g+rhs.g, b+rhs.b }; }
    void operator+=(const RGB& rhs) { r += rhs.r; g += rhs.g; b += rhs.b; }
    void operator/=(T rhs) { r /= rhs; g/= rhs; b /= rhs; }
};


template <typename T>
struct RGBA {
    union {
        struct { T r, g, b; };
        RGB<T> rgb = {};
    };
    T a = T{0};

    RGBA operator+(const RGBA& rhs) const { return { rgb+rhs.rgb, a+rhs.a}; }
    void operator+=(const RGBA& rhs) { rgb += rhs.rgb; a += rhs.a; }
    void operator/=(T rhs) { rgb /= rhs; a /= rhs; }

    RGBA& normalize() { rgb /= a; a = T{1}; return *this; }
};


template <typename T>
struct Image {
    Image(std::size_t width, std::size_t height): m_width(width), m_height(height), pixels(height * width) {}
    Image(const Image& other): m_width(other.m_width), m_height(other.m_height), pixels(other.pixels) {}
    Image(Image&& other): m_width(other.m_width), m_height(other.m_height), pixels(std::move(other.pixels)) {}

    Image(const std::vector<RGB<T>>& premultiplied, std::size_t width, std::size_t height)
        : m_width(width)
        , m_height(height)
        , pixels(std::ranges::to<std::vector<RGBA<T>>>(std::ranges::transform_view(premultiplied, [](const auto& elem){
            return (elem.r == T{0} && elem.g == T{0} && elem.b == T{0}) ? RGBA<T>{} : RGBA<T>{ .rgb = elem, .a = T{1} };
        })))
    {}

    RGB<T> averageNeighbors(std::size_t x, std::size_t y) const {
        RGBA<T> average;

        if (x > 0uz)
            average += { .rgb = this->operator[](x - 1uz, y).rgb, .a = T{1} };
        if (x < m_width - 1uz)
            average += { .rgb = this->operator[](x + 1uz, y).rgb, .a = T{1} };
        if (y > 0uz)
            average += { .rgb = this->operator[](x, y - 1uz).rgb, .a = T{1} };
        if (y < m_height - 1uz)
            average += { .rgb = this->operator[](x, y + 1uz).rgb, .a = T{1} };

        return average.normalize().rgb;
    }

    Image& operator=(Image&& rhs) = default;

    decltype(auto) operator[](this auto&& self, std::size_t x, std::size_t y) { return self.pixels[y*self.m_width + x]; }
    RGBA<T>* getPtr() { return pixels.data(); }

    std::vector<RGB<T>> convertToRGB() const { return std::ranges::to<std::vector<RGB<T>>>(std::ranges::transform_view(pixels, [](const RGBA<T>& elem){ return elem.rgb; })); }

public:
    const std::size_t m_width, m_height;
private:
    std::vector<RGBA<T>> pixels;
};
