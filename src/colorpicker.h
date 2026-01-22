#pragma once

#include <format>
#include <cstddef>


namespace ColorPicker {
    static constexpr std::size_t BCOLOR_BUFFER = 30u;

    template <typename... Args>
    void text(int x, int y, void* font, const std::format_string<Args...>& fmt, Args&&... args);

    void displayCurveColors();
    void mouseCurveColors(int button, int state, int x, int y);

    void displayPointColor();
    void mousePointColor(int button, int state, int x, int y);

    void reshape(int w, int h);
    void key(unsigned char c, int x, int y);
    void motion(int x, int y);
};
