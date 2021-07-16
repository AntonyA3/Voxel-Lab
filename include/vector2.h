#ifndef VECTOR2_H
#define VECTOR2_H
#include "../linmath/linmath.h"

typedef union{  
    struct{
        float x, y;
    };
    vec2 vec;
}Vector2;
#endif