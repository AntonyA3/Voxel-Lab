#ifndef RAY_H
#define RAY_H
#include "../linmath/linmath.h"
typedef union{
    struct
    {
        float x, y, z, dx, dy, dz;
    };
    struct
    {
        vec3 origin;
        vec3 direction;
    };
    float ray[6];
    
}Ray;
#endif