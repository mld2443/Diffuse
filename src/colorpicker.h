#pragma once

#include <format>  // format_string
#include <cstddef> // size_t


namespace ColorPicker {
    static constexpr std::size_t BCOLOR_BUFFER = 70u;

    template <typename... Args>
    void text(int x, int y, void* font, const std::format_string<Args...>& fmt, Args&&... args);

    void display();
    void mouse(int button, int state, int x, int y);
    void reshape(int w, int h);
    void motion(int x, int y);
};
