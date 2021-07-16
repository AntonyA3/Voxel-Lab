#ifndef VECTOR4_H
#define VECTOR4_H
#include "../linmath/linmath.h"
typedef union 
{
    struct 
    {
        float x, y, z, w;
    };
    struct 
    {
        vec3 xyz;
        float wComp;
    };
    vec4 vec;

}Vector4;
#endif