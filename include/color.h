#ifndef COLOR_H
#define COLOR_H
typedef union 
{
   float color[4];
   struct{
      float r, g, b, a;
   };
   
}ColorRGBAf;

typedef union 
{
   unsigned char color[4];
   struct{
      unsigned char r, g, b, a;
   };
   
}ColorRGBA;
void init_color_rgba_f(ColorRGBAf* col, float r, float g, float b, float a);
#endif