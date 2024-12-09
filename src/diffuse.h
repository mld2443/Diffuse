#pragma once

#include <GL/gl.h>


GLubyte* diffuse(uint32_t size, GLubyte *pixels, uint64_t smooth = 100, uint32_t limit = 512);
