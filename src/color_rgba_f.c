#include "../include/color_rgba_f.h"

void color_rgba_f_init(ColorRgbaF *color, float r, float g, float b, float a){
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}