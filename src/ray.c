#include "../include/ray.h"

void init_ray(Ray* ray, float x, float y, float z, float dx, float dy, float dz){
    ray->x = x;
    ray->y = y;
    ray->z = z;
    ray->dx = dx;
    ray->dy = dy;
    ray->dz = dz;
}