#ifndef PRIM_FACT_H
#define PRIM_FACT_H

#include <stdlib.h>
#include <stdio.h>
#include "linmath.h"

typedef struct Vertex{
    float x,y,z;
}Vertex;

void generate_cube_from_centre(vec3 center, vec3 halfExtents,  Vertex* vertexArray, unsigned int *elementArray);
void generate_cube_from_line(vec3 start, vec3 end, float thickness, 
    Vertex* vertexArray, unsigned int *elementArray);
#endif