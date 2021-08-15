#ifndef GRID_H
#define GRID_H
#include "../linmath/linmath.h"
typedef struct
{    
    vec3 min;
    vec3 vectorH;
    vec3 vectorV;
    int hCount;
    int vCount;
}Grid;
#endif
