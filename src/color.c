#include "../include/color.h"
/*
void init_ColorRGBAf(ColorRGBAf color, float r, float g, float b, float a){
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;

    printf("color r %f, g %f, b %f, a %f\n", color.r, color.g, color.b, color.a);

}*/
void init_color_rgba_f(ColorRGBAf* col, float r, float g, float b, float a){
    col->r = r;
    col->g = g;
    col->b = b;
    col->a = a;
}