#pragma once

#include <string>
#include <cstdint>


namespace ColorPicker {
    static const uint32_t BCOLOR_BUFFER = 30u;

    void print(const int x, const int y, void* font, const std::string s);

    void display_big();
    void mouse_big(int button, int state, int x, int y);

    void display_small();
    void mouse_small(int button, int state, int x, int y);

    void reshape(int w, int h);
    void key(unsigned char c, int x, int y);
    void motion(int x, int y);
};
