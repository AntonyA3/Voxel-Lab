#ifndef RECT_H
#define RECT_H
typedef struct rect
{
    float x, y, w, h;
}Rect;

int rect_contains_point(Rect rect, float x, float y);
#endif