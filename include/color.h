#ifndef COLOR_H
#define COLOR_H

#include <stdlib.h>
#include <string.h>
typedef union
{
    struct
    {
        float r, g, b, a;
    };
    float color[4];
}ColorRGBAF;

typedef union 
{
    struct 
    {
        unsigned char r, g, b, a;
    };
    unsigned char color[4];
}ColorRGBA;

ColorRGBAF colorRgba_to_colorRgbaf(ColorRGBA colorIn);
ColorRGBAF colorRgbaf_copy(ColorRGBAF *colorOut, ColorRGBAF colorIn);
void init_colorRgba(ColorRGBA *color, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
ColorRGBA colorRgbaf_to_colorRgba(ColorRGBAF colorIn);
ColorRGBA colorRgba_copy(ColorRGBA *colorOut, ColorRGBA colorIn);
void color_f_init(ColorRGBAF *color, float r, float g, float b, float a);
#endif