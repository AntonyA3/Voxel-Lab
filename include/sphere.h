#ifndef SPHERE_H
#define SPHERE_H
#include "../include/linmath.h"
#include "../include/aabb.h"
#include "../include/number_functions.h"
typedef struct
{
    float x, y, z, r;
}Sphere;

int sphere_contains_point(Sphere sphere, float x, float y, float z);
int sphere_contains_point_inclusive(Sphere sphere, float x, float y, float z);
int sphere_intersects_aabb(Sphere sphere, Aabb aabb);
void sphere_get_overlapping_area(Sphere sphere, Aabb aabb, vec3 overlap);
void sphere_get_box_max_point(Sphere sphere, vec3 point);
#endif