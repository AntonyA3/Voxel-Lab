
#include "../linmath/linmath.h"
typedef union
{
    struct{
        float x, y, z, r, g, b, a, nx, ny, nz;
    };
    float vertex[10];
    struct 
    {
        vec3 position;
        vec4 color;
        vec3 normal;
    };
    
}PosColor32NormalVertex;

