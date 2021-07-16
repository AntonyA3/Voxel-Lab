#ifndef VECTOR3_H
#define VECTOR3_H
#include "../linmath/linmath.h"

typedef union 
{
    struct 
    {
        float x, y, z;
    };
    vec3 vec;
}Vector3;
#endif