#ifndef SHAPE_H
#define SHAPE_H
#include "../include/linmath.h"

#define FLOATS_PER_CUBE 4 * 6 * 3
#define ELEMENTS_PER_CUBE 36
#define VERTICIES_PER_CUBE 24

extern float AABB_VERTEX_ARRAY[FLOATS_PER_CUBE];
extern float AABB_NORMAL_ARRAY[FLOATS_PER_CUBE];
extern unsigned int AABB_ELEMENTS_ARRAY[ELEMENTS_PER_CUBE];
enum shape{SHAPE_AABB, SHAPE_SPHERE};

typedef struct{
    float x, y, width, height;
}Rect;

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

typedef union
{
    struct
    {
        float x, y, z, r;
    };
    struct 
    {
        vec3 position;
        float radius;
    };
}Sphere;

typedef struct
{
    vec3 origin;
    vec3 halfExtents;
}Ovoid;

Rect rect_init(float x, float y, float width, float height);
int rect_contains_point(Rect rect, float x, float y);
int shape_intersects_aabb(int shape, void* shapeData, Aabb aabb);
int shape_contains_point(int shape, void* shapeData, float x, float y, float z);
Aabb shape_to_aabb(int shape, void* shapeData);

int aabb_contains_point(Aabb aabb, float x, float y, float z);
int aabb_intersects_aabb(Aabb aabb1, Aabb aabb2);
Aabb aabb_from_expand(Aabb aabb, float expansion);
void aabb_get_corners(Aabb aabb, vec3 corners[8]);
void aabb_get_origin(Aabb aabb, vec3 origin);

int sphere_intersects_aabb(Sphere sphere, Aabb aabb);
int sphere_contains_point(Sphere sphere, float x, float y, float z);
Aabb sphere_to_aabb(Sphere sphere);

int ovoid_contains_point(Ovoid ovoid, float x, float y, float z);
int ovoid_intersects_aabb(Ovoid ovoid, Aabb aabb);
#endif