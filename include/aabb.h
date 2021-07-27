#ifndef AABB_H
#define AABB_H
#include "../include/linmath.h"
#include "../include/number_functions.h"
typedef struct
{
    float x, y, z, w, h, d;
}Aabb;

void aabb_get_centre(Aabb aabb, vec3 centre);
void aabb_get_max_corner(Aabb aabb, vec3 point);
void aabb_get_overlaping_area(Aabb aabb1, Aabb aabb2, vec3 overlap);
void aabb_get_box_corners(Aabb aabb, vec3 corners[8]);
int aabb_contains_point(Aabb aabb, float x, float y, float z);
int aabb_contains_point_inclusive(Aabb aabb, float x, float y, float z);
int aabb_contains_aabb(Aabb aabbContainer, Aabb aabb);

Aabb aabb_get_box_with_subtracted_corners(Aabb aabb, float subtraction);

#endif
