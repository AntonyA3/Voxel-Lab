#ifndef DIRECTION_LIGHT_H
#define DIRECTION_LIGHT_H
#include "../linmath/linmath.h"
typedef struct
{   
    union
    {
        struct{
            float yaw, pitch;
        };
        vec2 orientation;
    };
    union{
        struct{
            float x, y, z;
        };
        vec3 direction;
    };
}DirectionLight;
#endif
