#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
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

int clamp(int l, int length) {
    // clamp width and height
    if ((l+1) > length) {
        l = length - 1; // 0 based indexing
    } else if (l < 0) {
        l = 0;
    }
    return l; 
}

int clamp_check(int l, int length) {
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
    float pix;
    for (int i=0; i<im.h; i++) {
        for (int j=0; j<im.w; j++) {
            pix = get_pixel(im,c,i,j);
            set_pixel(im,c,i,j,pix+v);
        }
    }
}       

void clamp_image(image im)
{
    int ind;
    float pix;
    for (int c=0; c<3; c++) {
        for (int i=0; i<im.h; i++) {
            for (int j=0; j<im.w; j++) {
                ind = get_index(im,c,i,j);
                pix = get_pixel(im,c,i,j);
                if (pix > 1) {
                    im.data[ind]=1;
                } else if (pix < 0) {
                    im.data[ind]=0;
                }
            }
        }
    }
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

// convert RGB image to Hue, Saturation, Value image
void rgb_to_hsv(image im)
{
    int index;
    int R_i,G_i,B_i;
    float R,G,B;
    float H,S,V;
    // Saturation vars
    float m,C;
    // Hue vars
    float H_dash;
    // compress to grayscale
    for (int i=0; i<im.h; i++) {
        for (int j=0; j<im.w; j++) {
            // get our R.G.B values from our color array
            R_i = get_index(im,0,i,j);
            G_i = get_index(im,1,i,j);
            B_i = get_index(im,2,i,j);
            R = im.data[R_i];
            G = im.data[G_i];
            B = im.data[B_i];
            V = three_way_max(R,G,B);
            if ((R==0) && (G==0) && (B==0)) {
                // avoid divide by zero
                S = 0;
                C = 0;
            } else {
                m = three_way_min(R,G,B);
                C = V - m;
                S = C / V;
            }
            if (C==0) {
                H = 0;
            } else {
                if (V==R) {
                    H_dash = (G-B)/C;
                } else if (V==G) {
                    H_dash = (B-R)/C + 2;
                } else {
                    H_dash = (R-G)/C + 4;
                }
                if (H_dash < 0) {
                    H = H_dash/6 + 1;
                } else {
                    H = H_dash/6;
                }
            }
            // Now set R,G,B to H,S,V
            im.data[R_i] = H;
            im.data[G_i] = S;
            im.data[B_i] = V;
        }
    }
}

void hsv_to_rgb(image im)
{
    int index;
    int H_i,S_i,V_i;
    float R,G,B;
    float H,S,V;
    float max,C,min,H_;
    // compress to grayscale
    for (int i=0; i<im.h; i++) {
        for (int j=0; j<im.w; j++) {
            // get our R.G.B values from our color array
            H_i = get_index(im,0,i,j);
            S_i = get_index(im,1,i,j);
            V_i = get_index(im,2,i,j);
            H = im.data[H_i];
            S = im.data[S_i];
            V = im.data[V_i];
            // calculate R,G,B from HSV
            // with horrific formula
            // using code from https://github.com/DiaaZiada/Uwimg/blob/master/src/process_image.c
            C = V * S;
            max = V;
            min = V-C;

            H_ = H * 6 ;

            if(C == 0.){
                R = V;
                G = V;
                B = V;
            }else if(H_ > 5 && H_ <6){
                R = max;
                G = min;
                B = ((((H_ /  6) - 1) * 6 * C) - G) * -1;
            }else if (H_ == 5) {
                R = max;
                G = min;
                B = max;
            }else if (H_ < 5 && H_ > 4) {
                G = min;
                R = (H_ - 4) * C + G;
                B = max;
            }else if (H_ == 4) {
                R = min;
                G = min;
                B = max;
            }else if (H_ < 4 && H_ > 3) {
                R = min;
                G = (((H_ - 4) * C) - R) * -1;
                B = max;
            }else if (H_ == 3) {
                R = min;
                G = max;
                B = max;
            }else if (H_ < 3 && H_ > 2) {
                R = min;
                G = max;
                B = ((H_ - 2) * C) + R;
            }else if (H_ == 2) {
                R = min;
                G = max;
                B = min;
            }else if (H_ < 2 && H_ > 1) {
                G = max;
                B = min;
                R = (((H_ - 2) * C) - B) * -1;
            }else if (H_ == 1) {
                R = max;
                G = max;
                B = min;
            }else if (H_ < 1 && H_ > 0) {
                R = max;
                B = min;
                G = (H_ * C) + B;
            }else {
                R = max;
                G = min;
                B = min;
            }

            // Now set HSV to R,G,B
            im.data[H_i] = R;
            im.data[S_i] = G;
            im.data[V_i] = B;
        }
    }
}
