#include "../include/rect.h"

int rect_contains_point(Rect rect, float x, float y){
    return (x >= rect.x) && (x < (rect.x + rect.w)) &&
        (y >= rect.y) && (y < (rect.y + rect.h));
}