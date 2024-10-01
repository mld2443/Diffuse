#pragma once

#include <GL/gl.h>
#include <cstdint>


GLubyte getpixel(const GLubyte *pixels, uint64_t size, uint64_t x, uint64_t y, uint32_t c);

bool isblack(const GLubyte *pixels, uint64_t size, uint64_t x, uint64_t y);

void setpixel(GLubyte *pixels, uint64_t size, uint64_t x, uint64_t y, uint64_t c, GLubyte val);

GLubyte neighbors(const GLubyte *pixels, uint64_t size, uint64_t x, uint64_t y, uint32_t c);

GLubyte* downsample(uint32_t size, GLubyte *pixels);

GLubyte* upscale(uint32_t size, GLubyte *lowres, GLubyte *highres, uint64_t smooth, uint32_t limit);

GLubyte* diffuse(uint32_t size, GLubyte *pixels, uint64_t smooth = 100, uint32_t limit = 512);
