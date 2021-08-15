#ifndef SHAPE_H
#define SHAPE_H
#include "../include/aabb.h"
#include "../include/sphere.h"
enum shape{SHAPE_AABB, SHAPE_SPHERE};

int shape_intersects_aabb(int shape, void* shapeData, Aabb aabb);
int shape_contains_point(int shape, void* shapeData, float x, float y, float z);
Aabb shape_to_aabb(int shape, void* shapeData);
#endif