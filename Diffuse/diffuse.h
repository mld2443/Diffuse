//
//  diffuse.h
//  Diffuse
//
//  Created by Matthew Dillard on 10/28/15.
//  Copyright © 2015 Matthew Dillard. All rights reserved.
//

#ifndef diffuse_h
#define diffuse_h

typedef unsigned long ul;
GLubyte getpixel(const GLubyte *pixels, const ul size, const ul x, const ul y, const ui c) {
    return pixels[((size*y)+x)*3+c];
}

bool isblack(const GLubyte *pixels, const ul size, const ul x, const ul y) {
    if (getpixel(pixels, size, x, y, 0))
        return false;
    if (getpixel(pixels, size, x, y, 1))
        return false;
    if (getpixel(pixels, size, x, y, 2))
        return false;
    return true;
}

void setpixel(GLubyte *pixels, const ul size, const ul x, const ul y, const ul c, const GLubyte val) {
    pixels[((size*y)+x)*3+c] = val;
}

GLubyte neighbors(const GLubyte *pixels, const ul size, const ul x, const ul y, const ui c) {
    ui total = 0, rvalue = 0;
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

GLubyte* downsample(ui size, GLubyte *pixels) {
    ui newsize = size/2;
    GLubyte *newpixels = new GLubyte[newsize * newsize * 3];
    
    int r,g,b, sumr,sumg,sumb;
    int total;
    for (ul y = 0; y < size; y += 2) {
        for (ul x = 0; x < size; x += 2) {
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

GLubyte* upscale(ui size, GLubyte *lowres, GLubyte *highres, const ul smooth, const ui limit) {
    auto constrained = vector<vector<bool>>(size,vector<bool>(size, true));
    for (ul x = 0; x < size; x+=2) {
        for (ul y = 0; y < size; y+=2) {
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
        for (ul step = 0; step < smooth; step++)
            for (ul x = 0; x < size; x++)
                for (ul y = 0; y < size; y++)
                    if (!constrained[y][x]) {
                        setpixel(highres, size, x, y, 0, neighbors(highres, size, x, y, 0));
                        setpixel(highres, size, x, y, 1, neighbors(highres, size, x, y, 1));
                        setpixel(highres, size, x, y, 2, neighbors(highres, size, x, y, 2));
                    }
    
    return highres;
}

GLubyte* diffuse(const ui size, GLubyte *pixels, const ul smooth = 100, const ui limit = 512) {
    if (size <= 1)
        return pixels;
    
    return upscale(size, diffuse(size/2, downsample(size, pixels), smooth, limit), pixels, smooth, limit);
}

#endif /* diffuse_h */
