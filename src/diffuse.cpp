#include "diffuse.h"

#include <vector>

using namespace std;


GLubyte getpixel(const GLubyte *pixels, uint64_t size, uint64_t x, uint64_t y, uint32_t c) {
    return pixels[((size*y)+x)*3+c];
}

bool isblack(const GLubyte *pixels, uint64_t size, uint64_t x, uint64_t y) {
    if (getpixel(pixels, size, x, y, 0))
        return false;
    if (getpixel(pixels, size, x, y, 1))
        return false;
    if (getpixel(pixels, size, x, y, 2))
        return false;
    return true;
}

void setpixel(GLubyte *pixels, uint64_t size, uint64_t x, uint64_t y, uint64_t c, GLubyte val) {
    pixels[((size*y)+x)*3+c] = val;
}

GLubyte neighbors(const GLubyte *pixels, uint64_t size, uint64_t x, uint64_t y, uint32_t c) {
    uint32_t total = 0, rvalue = 0;
    if(x > 0){
        rvalue += getpixel(pixels, size, x-1, y, c);
        total++;
    }
    if(x < size - 1){
        rvalue += getpixel(pixels, size, x+1, y, c);
        total++;
    }
    if(y > 0){
        rvalue += getpixel(pixels, size, x, y-1, c);
        total++;
    }
    if(y < size - 1){
        rvalue += getpixel(pixels, size, x, y+1, c);
        total++;
    }
    return rvalue/total;
}

GLubyte* downsample(uint32_t size, GLubyte *pixels) {
    uint32_t newsize = size/2;
    GLubyte *newpixels = new GLubyte[newsize * newsize * 3];

    int r,g,b, sumr,sumg,sumb;
    int total;
    for (uint64_t y = 0; y < size; y += 2) {
        for (uint64_t x = 0; x < size; x += 2) {
            total = 0;
            sumr = r = getpixel(pixels, size, x, y, 0);
            sumg = g = getpixel(pixels, size, x, y, 1);
            sumb = b = getpixel(pixels, size, x, y, 2);
            if (r + g + b > 0)
                total++;

            sumr += r = getpixel(pixels, size, x+1, y, 0);
            sumg += g = getpixel(pixels, size, x+1, y, 1);
            sumb += b = getpixel(pixels, size, x+1, y, 2);
            if (r + g + b > 0)
                total++;

            sumr += r = getpixel(pixels, size, x, y+1, 0);
            sumg += g = getpixel(pixels, size, x, y+1, 1);
            sumb += b = getpixel(pixels, size, x, y+1, 2);
            if (r + g + b > 0)
                total++;

            sumr += r = getpixel(pixels, size, x+1, y+1, 0);
            sumg += g = getpixel(pixels, size, x+1, y+1, 1);
            sumb += b = getpixel(pixels, size, x+1, y+1, 2);
            if (r + g + b > 0)
                total++;

            if (total) {
                sumr /= total;
                sumg /= total;
                sumb /= total;
            }

            setpixel(newpixels, newsize, x/2, y/2, 0, sumr);
            setpixel(newpixels, newsize, x/2, y/2, 1, sumg);
            setpixel(newpixels, newsize, x/2, y/2, 2, sumb);
        }
    }

    return newpixels;
}

GLubyte* upscale(uint32_t size, GLubyte *lowres, GLubyte *highres, uint64_t smooth, uint32_t limit) {
    auto constrained = vector<vector<bool>>(size,vector<bool>(size, true));
    for (uint64_t x = 0; x < size; x+=2) {
        for (uint64_t y = 0; y < size; y+=2) {
            if (isblack(highres, size, x, y)) {
                constrained[y][x] = false;
                setpixel(highres, size, x, y, 0, getpixel(lowres, size/2, x/2, y/2, 0));
                setpixel(highres, size, x, y, 1, getpixel(lowres, size/2, x/2, y/2, 1));
                setpixel(highres, size, x, y, 2, getpixel(lowres, size/2, x/2, y/2, 2));
            }
            if (isblack(highres, size, x+1, y)) {
                constrained[y][x+1] = false;
                setpixel(highres, size, x+1, y, 0, getpixel(lowres, size/2, x/2, y/2, 0));
                setpixel(highres, size, x+1, y, 1, getpixel(lowres, size/2, x/2, y/2, 1));
                setpixel(highres, size, x+1, y, 2, getpixel(lowres, size/2, x/2, y/2, 2));
            }
            if (isblack(highres, size, x, y+1)) {
                constrained[y+1][x] = false;
                setpixel(highres, size, x, y+1, 0, getpixel(lowres, size/2, x/2, y/2, 0));
                setpixel(highres, size, x, y+1, 1, getpixel(lowres, size/2, x/2, y/2, 1));
                setpixel(highres, size, x, y+1, 2, getpixel(lowres, size/2, x/2, y/2, 2));
            }
            if (isblack(highres, size, x+1, y+1)) {
                constrained[y+1][x+1] = false;
                setpixel(highres, size, x+1, y+1, 0, getpixel(lowres, size/2, x/2, y/2, 0));
                setpixel(highres, size, x+1, y+1, 1, getpixel(lowres, size/2, x/2, y/2, 1));
                setpixel(highres, size, x+1, y+1, 2, getpixel(lowres, size/2, x/2, y/2, 2));
            }
        }
    }

    delete [] lowres;

    if (false)
        for (uint64_t step = 0; step < smooth; step++)
            for (uint64_t x = 0; x < size; x++)
                for (uint64_t y = 0; y < size; y++)
                    if (!constrained[y][x]) {
                        setpixel(highres, size, x, y, 0, neighbors(highres, size, x, y, 0));
                        setpixel(highres, size, x, y, 1, neighbors(highres, size, x, y, 1));
                        setpixel(highres, size, x, y, 2, neighbors(highres, size, x, y, 2));
                    }

    return highres;
}

GLubyte* diffuse(uint32_t size, GLubyte *pixels, uint64_t smooth, uint32_t limit) {
    if (size <= 1)
        return pixels;

    return upscale(size, diffuse(size/2, downsample(size, pixels), smooth, limit), pixels, smooth, limit);
}
