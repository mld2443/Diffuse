#pragma once

#include "image.h"

#include <cstddef>


template<typename T>
Image<T> downscale(const Image<T>& fromLarger) {
    Image<T> toSmaller(fromLarger.m_width/2uz, fromLarger.m_height/2uz);

    //auto kernel = Kernel{ { { 1.f, 1.f}, { 1.f, 1.f }, }, };

    for (std::size_t y = 0uz; y < fromLarger.m_height; y += 2uz) {
        for (std::size_t x = 0uz; x < fromLarger.m_width; x += 2uz) {
            RGBA<T> average;

            average += fromLarger[x    , y    ];
            average += fromLarger[x+1uz, y    ];
            average += fromLarger[x    , y+1uz];
            average += fromLarger[x+1uz, y+1uz];

            if (average.a > T{0})
                average.normalize();

            toSmaller[x/2uz, y/2uz] = average;
        }
    }

    return toSmaller;
}

template<typename T>
Image<T> upscale(const Image<T>&& fromSmaller, Image<T>& toLarger) {
    for (std::size_t y = 0uz; y < toLarger.m_height; y+=2uz) {
        for (std::size_t x = 0uz; x < toLarger.m_width; x+=2uz) {
            if (toLarger[x    , y    ].a == T{0})
                toLarger[x    , y    ].rgb = fromSmaller[x/2uz, y/2uz].rgb;
            if (toLarger[x+1uz, y    ].a == T{0})
                toLarger[x+1uz, y    ].rgb = fromSmaller[x/2uz, y/2uz].rgb;
            if (toLarger[x    , y+1uz].a == T{0})
                toLarger[x    , y+1uz].rgb = fromSmaller[x/2uz, y/2uz].rgb;
            if (toLarger[x+1uz, y+1uz].a == T{0})
                toLarger[x+1uz, y+1uz].rgb = fromSmaller[x/2uz, y/2uz].rgb;
        }
    }

    return toLarger;
}

template<typename T>
Image<T> maskedBlur(Image<T>&& buffer, std::size_t iterations) {
    for (std::size_t step = 0uz; step < iterations; ++step)
        for (std::size_t y = 0uz; y < buffer.m_height; ++y)
            for (std::size_t x = 0uz; x < buffer.m_width; ++x)
                if (buffer[x, y].a == T{0})
                    buffer[x, y].rgb = buffer.averageNeighbors(x, y);

    return buffer;
}

template<typename T>
Image<T> pyramidDiffusion(Image<T>&& buffer, std::size_t iterations) {
    if (buffer.m_width <= 1uz || buffer.m_height <= 1uz)
        return buffer;

    return maskedBlur(upscale(pyramidDiffusion(downscale(buffer), iterations), buffer), iterations);
}
