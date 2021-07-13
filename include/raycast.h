#include "linmath.h"

#ifndef RAY_H
#define RAY_H
#include "../include/ray.h"
#endif

#ifndef VOXEL_H
#define VOXEL_H
#include "../include/voxel.h"
#endif

#ifndef AABB_H
#define AABB_H
#include "../include/aabb.h"
#endif

#ifndef AABB_FACT_H
#define AABB_FACT_H
#include "../include/aabb_fact.h"
#endif

void ray_vs_voxel_grid(Voxel* voxel, Ray ray, int *didHit, vec3 hitpoint, int *sideHit);
void ray_vs_aabb(Aabb box, Ray ray, int *didHit, vec3 hitpoint,  int *sideHit);