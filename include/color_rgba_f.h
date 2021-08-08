#ifndef COLOR_RGBA_F_H
#define COLOR_RGBA_F_H
#include "../include/linmath.h"
typedef union{    
    vec4 color;
    struct{
        float r, g, b, a;
    };
}ColorRgbaF;
#endif