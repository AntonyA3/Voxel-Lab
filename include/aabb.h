#ifndef AABB_H
#define AABB_H
#define FLOATS_PER_CUBE 4 * 6 * 3
#define ELEMENTS_PER_CUBE 36
#define VERTICIES_PER_CUBE 24
#include "../include/linmath.h"
typedef union 
{   
    struct{
        float x, y, z;
        float width, height, depth;
    };
    struct 
    {
        vec3 min;
        vec3 size;
    };
    
}Aabb;

extern float AABB_VERTEX_ARRAY[FLOATS_PER_CUBE];
extern float AABB_NORMAL_ARRAY[FLOATS_PER_CUBE];
extern unsigned int AABB_ELEMENTS_ARRAY[ELEMENTS_PER_CUBE];

int aabb_contains_point(Aabb aabb, float x, float y, float z);
int aabb_intersects_aabb(Aabb aabb1, Aabb aabb2);
Aabb aabb_from_expand(Aabb aabb, float expansion);
void aabb_get_corners(Aabb aabb, vec3 corners[8]);
void aabb_get_origin(Aabb aabb, vec3 origin);
#endif