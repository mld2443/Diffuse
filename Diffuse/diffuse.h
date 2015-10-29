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
GLubyte getpixel(const GLubyte *pixels, const ul size, const ul x, const ul y, const ul c) {
    return pixels[((size*y)+x)*3+c];
}

void setpixel(GLubyte *pixels, const ul size, const ul x, const ul y, const ul c, const GLubyte val) {
    pixels[((size*y)+x)*3+c] = val;
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

GLubyte* upscale(ui size, GLubyte *pixels) {
    ui newsize = size*2;
    GLubyte *newpixels = new GLubyte[newsize * newsize * 3];
    
    for (ul x = 0; x < size; x++) {
        for (ul y = 0; y < size; y++) {
            for (int i = 0; i < 3; i++) {
                setpixel(newpixels, newsize, x*2, y*2, i, getpixel(pixels, size, x, y, i));
                setpixel(newpixels, newsize, x*2+1, y*2, i, getpixel(pixels, size, x, y, i));
                setpixel(newpixels, newsize, x*2, y*2+1, i, getpixel(pixels, size, x, y, i));
                setpixel(newpixels, newsize, x*2+1, y*2+1, i, getpixel(pixels, size, x, y, i));
            }
        }
    }
    
    return newpixels;
}

GLubyte* diffuse(const ui size, GLubyte *pixels, const ui iter) {
    if (iter == 0)
        return pixels;
    
    return upscale(size/2, diffuse(size/2, downsample(size, pixels), iter - 1));
}

#endif /* diffuse_h */
