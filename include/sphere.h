#include <math.h>
#include "../linmath/linmath.h"
#include "../include/aabb.h"
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

int sphere_intersects_aabb(Sphere sphere, Aabb aabb);
int sphere_contains_point(Sphere sphere, float x, float y, float z);
Aabb sphere_to_aabb(Sphere sphere);