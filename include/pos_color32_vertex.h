#ifndef POS_COLOR32_VERTEX_H
#define POS_COLOR32_VERTEX_H
#include "../linmath/linmath.h"
typedef union
{
    struct{
        float x, y, z, r, g, b, a;
    };
    float vertex[7];
    struct 
    {
        vec3 position;
        vec4 color;
    };
    
}PosColor32Vertex;


#endif