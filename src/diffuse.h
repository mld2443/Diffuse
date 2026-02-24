#pragma once

#include "image.h"

#include <cstddef>


template<typename T>
Image<T> downscale(const Image<T>& fromLarger) {
    Image<T> toSmaller(fromLarger.m_width >> 1uz, fromLarger.m_height >> 1uz);

    for (std::size_t Y = 0uz; Y < fromLarger.m_height; ++Y)
        for (std::size_t X = 0uz; X < fromLarger.m_width; ++X)
            toSmaller[X >> 1uz, Y >> 1uz] += fromLarger[X, Y];

    for (std::size_t y = 0uz; y < toSmaller.m_height; ++y)
        for (std::size_t x = 0uz; x < toSmaller.m_width; ++x)
            if (toSmaller[x, y].a > T{0})
                toSmaller[x, y] = toSmaller[x, y].normalize();

    return toSmaller;
}

template<typename T>
Image<T> upscale(const Image<T>&& fromSmaller, const Image<T>& mask) {
    Image<T> toLarger{mask.m_width, mask.m_height};

    for (std::size_t Y = 0uz; Y < toLarger.m_height; ++Y)
        for (std::size_t X = 0uz; X < toLarger.m_width; ++X)
            toLarger[X, Y] = mask[X, Y].a > T{0} ? mask[X, Y] : fromSmaller[X >> 1uz, Y >> 1uz];

    return toLarger;
}

template<typename T>
Image<T> maskedBlur(Image<T>&& image, const Image<T>& mask, std::size_t iterations, typename Image<T>::EdgePolicy policy) {
    static constexpr Kernel kernel{ { { 0.f, 1.f, 0.f, },
                                      { 1.f, 1.f, 1.f, },
                                      { 0.f, 1.f, 0.f, }, } };

    FlipBuffer buffers{ std::move(image) };

    // copy over masked parts
    for (std::size_t y = 0uz; y < buffers.height(); ++y)
        for (std::size_t x = 0uz; x < buffers.width(); ++x)
            if (mask[x, y].a > T{0})
                buffers.dst()[x, y] = buffers.src()[x, y];

    // blur unmasked parts
    for (std::size_t step = 0uz; step < iterations; ++step) {
        for (std::size_t y = 0uz; y < buffers.height(); ++y)
            for (std::size_t x = 0uz; x < buffers.width(); ++x)
                if (mask[x, y].a <= T{0})
                    buffers.dst()[x, y] = buffers.src().sampleKernel(x, y, kernel, policy);
        buffers.flip();
    }

    // src() is const lvalue to prevent mistakes above, but now it's time to go, no need to copy
    return std::move(const_cast<Image<T>&>(buffers.src()));
}

template<typename T>
Image<T> pyramidDiffusion(Image<T>&& buffer, std::size_t iterations, typename Image<T>::EdgePolicy policy, std::size_t depth) {
    if (depth <= 0uz || buffer.m_width <= 1uz || buffer.m_height <= 1uz)
        return maskedBlur(Image<T>{buffer}, buffer, iterations, policy);

    return maskedBlur(upscale(pyramidDiffusion(downscale(buffer), iterations, policy, depth - 1uz), buffer), buffer, iterations, policy);
}
