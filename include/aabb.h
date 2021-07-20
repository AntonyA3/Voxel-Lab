#ifndef AABB_H
#define AABB_H
#include "vector3.h"
#include "ray.h"
#include "../include/sphere.h"


enum box_sides{BOX_SIDE_LEFT, BOX_SIDE_RIGHT, BOX_SIDE_BOTTOM, BOX_SIDE_TOP, BOX_SIDE_FRONT, BOX_SIDE_BACK};

typedef struct Aabb{
    float x, y, z, width, height,depth;
}Aabb;

int aabb_contains_point(Aabb box, float x, float y, float z);
void aabb_vs_ray(Aabb aabb, Ray ray, int *hit, int *side, float *hitDistance);
int aabb_inside_sphere(Aabb aabb, Sphere sphere);
int aabb_intersects_sphere(Aabb aabb, Sphere sphere);
#endif