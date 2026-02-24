#pragma once

#include "types.h"

#include <cstdio>     // FILE
#include <filesystem> // filesystem::path
#include <string>     // string
#include <vector>     // vector

#include <png.h>  // png_byte


enum class PixelFormat {
    GRAY = PNG_COLOR_TYPE_GRAY,
    RGB  = PNG_COLOR_TYPE_RGB,
    RGBA = PNG_COLOR_TYPE_RGBA,
};


class PNGWriter {
public:
    PNGWriter(std::string&& filename);
    ~PNGWriter();

    PNGWriter(const PNGWriter&) = delete;
    PNGWriter& operator=(const PNGWriter&) = delete;

    void write(std::vector<png_byte>& pixels, PixelFormat format, size_t width, size_t height, size_t bitDepth = 8uz);

private:
    std::filesystem::path m_path;
    FILE*                 m_file;
};
