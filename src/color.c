#include "../include/color.h"

ColorRGBAF colorRgba_to_colorRgbaf(ColorRGBA colorIn){
    ColorRGBAF color;
    for(int i = 0; i < 4; i++){
        color.color[i] = (colorIn.color[i] / 255.0f);
    }
    return color;
}

void init_colorRgba(ColorRGBA *color, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}
ColorRGBAF colorRgbaf_copy(ColorRGBAF *colorOut, ColorRGBAF colorIn){
    memcpy(colorOut, &colorIn, sizeof(ColorRGBAF));

}

ColorRGBA colorRgba_copy(ColorRGBA *colorOut, ColorRGBA colorIn){
    memcpy(colorOut,  &colorIn, sizeof(ColorRGBA));
}


ColorRGBA colorRgbaf_to_colorRgba(ColorRGBAF colorIn){
    ColorRGBA color;
    for(int i = 0; i < 4; i++){
        color.color[i] = (unsigned char)(colorIn.color[i] * 255);
    }
    return color;
}

void color_f_init(ColorRGBAF *color, float r, float g, float b, float a){
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}
