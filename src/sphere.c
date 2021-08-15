#include "../include/sphere.h"

int sphere_intersects_aabb(Sphere sphere, Aabb aabb){
    vec3 sphereToAabb;
    vec3 aabbCenter;
    aabb_get_origin(aabb, aabbCenter);
    vec3_sub(sphereToAabb, aabbCenter, sphere.position);

    if(sphere_contains_point(sphere, aabbCenter[0], aabbCenter[1], aabbCenter[2])){
        return 1;
    }
    
    if(aabb_contains_point(aabb, sphere.x, sphere.y, sphere.z)){
        return 1;
    }
    {
        float dLeft = abs(aabb.x - sphere.x);
        float dRight = abs(aabb.x + aabb.width - sphere.x);
        vec3 p = {
            aabb.x * (dLeft <= dRight) + (aabb.x + aabb.width) * (dRight < dLeft), 
            fmaxf(fminf(sphere.y, aabb.y + aabb.height), aabb.y), 
            fmaxf(fminf(sphere.z, aabb.z + aabb.depth), aabb.z)
        };
        if(sphere_contains_point(sphere, p[0], p[1], p[2])){
            return 1;
        }
    }
    {  
        float dBottom = abs(aabb.y - sphere.y);
        float dTop = abs(aabb.y + aabb.height - sphere.y);
        vec3 p = {
            fmaxf(fminf(sphere.x, aabb.x + aabb.width), aabb.x),
            aabb.y * (dBottom <= dTop) + (aabb.y + aabb.height) * (dTop < dBottom), 
            fmaxf(fminf(sphere.z, aabb.z + aabb.depth), aabb.z)

        };
        if(sphere_contains_point(sphere, p[0], p[1], p[2])){
            return 1;
        }
    }
    {
        float dFront = abs(aabb.z - sphere.z);
        float dBack = abs(aabb.z + aabb.depth - sphere.z);
        vec3 p = {
            fmaxf(fminf(sphere.x, aabb.x + aabb.width), aabb.x), 
            fmaxf(fminf(sphere.y, aabb.y + aabb.height), aabb.y), 
            aabb.z * (dFront <= dBack) + (aabb.z + aabb.depth) * (dBack < dFront) 
        };
        if(sphere_contains_point(sphere, p[0], p[1], p[2])){
            return 1;
        }
    }
    return 0;
}

int sphere_contains_point(Sphere sphere, float x, float y, float z){
    vec3 point = {x, y, z};
    vec3 pointToSphere;
    vec3_sub(pointToSphere, point, sphere.position);
    return vec3_mul_inner(pointToSphere, pointToSphere) <= sphere.r * sphere.r;
}

Aabb sphere_to_aabb(Sphere sphere){
    float d = 2 * sphere.r;
    Aabb aabb = {sphere.x - sphere.r, sphere.y - sphere.r, sphere.z - sphere.r,
        d,d,d
    };
    return aabb;
}