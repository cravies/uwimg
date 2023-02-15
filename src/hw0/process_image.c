#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

int get_index(image im, int c,int h,int w) {
    // get index in CHW format
    // i.e channel, row, column
    // so data[0] = channel 0, row 0, column 0
    // data[1] = channel 0, row 0, column 1
    // data[2] = channel 0, row 0, column 2
    // so index of c,h,w will be = c * H * W + h * W + w
    int index = (c * im.h * im.w) + (h * im.w) + w;
    return index;
}

int clamp(l, length) {
    // clamp width and height
    if ((l+1) > length) {
        l = length - 1; // 0 based indexing
    } else if (l < 0) {
        l = 0;
    }
    return l; 
}

int clamp_check(l, length) {
    // are we clamping?
    if ((l+1) > length || l < 0) {
        return 1;
    }
    return 0;
}

float get_pixel(image im, int c, int h, int w)
{
    float pixel;
    int width = im.w;
    int height = im.h;
    int index;
    h = clamp(h,height);
    w = clamp(w,width);
    // grab pixel
    // stored in C,H,W
    index = get_index(im, c, h, w);
    pixel = im.data[index];
    return pixel;
}

void set_pixel(image im, int c, int h, int w, float v)
{
    int index;
    if (clamp_check(h,im.h) || clamp_check(w,im.w)) {
        return;
    } else {
        index = get_index(im, c, h, w);
        im.data[index] = v;
    }
}

image copy_image(image im)
{
    image copy = make_image(im.c, im.h, im.w);
    // length in memory
    int len = sizeof(float) * (im.c*im.h*im.w);
    memcpy(copy.data, im.data, len);
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(1, im.h, im.w);
    int index;
    float R,G,B;
    // compress to grayscale
    for (int i=0; i<im.h; i++) {
        for (int j=0; j<im.w; j++) {
            // get index for our gray array
            index = get_index(gray,0,i,j);
            // get our R.G.B values from our color array
            R = im.data[get_index(im,0,i,j)];
            G = im.data[get_index(im,1,i,j)];
            B = im.data[get_index(im,2,i,j)];
            gray.data[index] = 0.299*R + 0.587*G + 0.114*B;
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
}

void clamp_image(image im)
{
    // TODO Fill this in
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
}
