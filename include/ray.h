#ifndef RAY_H
#define RAY_H
#include "../linmath/linmath.h"

typedef union{    
    float ray[6];
    struct{
        float x, y , z ,dx ,dy ,dz;
    };
    struct
    {
        vec3 origin;
        vec3 direction;
    };
}Ray;

void init_ray(Ray* ray, float x, float y, float z, float dx, float dy, float dz);
#endif