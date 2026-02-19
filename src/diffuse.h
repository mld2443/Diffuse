#pragma once

#include "image.h"

#include <cstddef>


template<typename T>
Image<T> downscale(const Image<T>& fromLarger) {
    Image<T> toSmaller(fromLarger.m_width >> 1uz, fromLarger.m_height >> 1uz);

    for (std::size_t y = 0uz; y < fromLarger.m_height; ++y)
        for (std::size_t x = 0uz; x < fromLarger.m_width; ++x)
            toSmaller[x >> 1uz, y >> 1uz] += fromLarger[x, y];

    for (std::size_t y = 0uz; y < toSmaller.m_height; ++y)
        for (std::size_t x = 0uz; x < toSmaller.m_width; ++x)
            if (toSmaller[x, y].a > T{0})
                toSmaller[x, y] = toSmaller[x, y].normalize();

    return toSmaller;
}

template<typename T>
Image<T> upscale(const Image<T>&& fromSmaller, Image<T>& toLarger) {
    for (std::size_t y = 0uz; y < toLarger.m_height; ++y)
        for (std::size_t x = 0uz; x < toLarger.m_width; ++x)
            if (toLarger[x, y].a == T{0})
                toLarger[x, y].rgb = fromSmaller[x >> 1uz, y >> 1uz].rgb;

    return toLarger;
}

template<typename T>
Image<T> maskedBlur(Image<T>&& buffer, std::size_t iterations, std::size_t blurUpTo) {
    if (buffer.m_width > blurUpTo && buffer.m_height > blurUpTo)
        return buffer;

    static constexpr Kernel kernel{ { { 0.f, 1.f, 0.f, },
                                      { 1.f, 1.f, 1.f, },
                                      { 0.f, 1.f, 0.f, }, } };

    for (std::size_t step = 0uz; step < iterations; ++step)
        for (std::size_t y = 0uz; y < buffer.m_height; ++y)
            for (std::size_t x = 0uz; x < buffer.m_width; ++x)
                if (buffer[x, y].a == T{0})
                    buffer[x, y].rgb = buffer.sample(x, y, kernel);

    return buffer;
}

template<typename T>
Image<T> pyramidDiffusion(Image<T>&& buffer, std::size_t iterations, std::size_t blurUpTo) {
    if (buffer.m_width <= 1uz || buffer.m_height <= 1uz)
        return buffer;

    return maskedBlur(upscale(pyramidDiffusion(downscale(buffer), iterations, blurUpTo), buffer), iterations, blurUpTo);
}
