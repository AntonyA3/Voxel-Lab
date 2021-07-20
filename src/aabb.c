#include "../include/aabb.h"
int aabb_contains_point(Aabb box, float x, float y, float z){
    int minX = box.x;
    int maxX = box.x + box.width;
    int minY = box.y;
    int maxY = box.y + box.height;
    int minZ = box.z;
    int maxZ = box.z + box.depth;
    return (x >= minX) && (x < maxX) && 
        (y >= minY) && (y < maxY) &&
        (z >= minZ) && (z < maxZ);
}


void aabb_vs_ray(Aabb aabb, Ray ray, int *hit, int *side, float *hitDistance){
    
    float x, y, z, d;

    if(ray.dx != 0){
        if(ray.dx > 0){
            d = (aabb.x - ray.x) / ray.dx; 
            x = aabb.x;
        }else if(ray.dx < 0){
            d = (aabb.x + aabb.width - ray.x) / ray.dx; 
            x = aabb.x + aabb.width;
        }
        y = ray.y + ray.dy * d;
        z = ray.z + ray.dz * d;
        int pred = (d >= 0 ) && 
            (y >= aabb.y) && (y < aabb.y + aabb.height) &&
            (z >= aabb.z) && (z < aabb.z + aabb.depth);
        if(pred){
            if(!(*hit) || d < (*hitDistance)){
                *hitDistance = d;
                *hit = 1;
                *side = BOX_SIDE_LEFT * (ray.dx > 0) +  BOX_SIDE_RIGHT * (ray.dx < 0);
            }
        }
    }

    if(ray.dy != 0){
        if(ray.dy > 0){
            d = (aabb.y - ray.y) / ray.dy; 
            y = aabb.y;
        }else if(ray.dy < 0){
            d = (aabb.y + aabb.height - ray.y) / ray.dy; 
            y = aabb.y + aabb.height;
        }
        x = ray.x + ray.dx * d;
        z = ray.z + ray.dz * d;
        int pred = (d >= 0 ) && 
            (x >= aabb.x) && (x < aabb.x + aabb.width) &&
            (z >= aabb.z) && (z < aabb.z + aabb.depth);
        if(pred){
            if(!(*hit) || d < (*hitDistance)){
                *hitDistance = d;
                *hit = 1;
                *side = BOX_SIDE_BOTTOM * (ray.dy > 0) +  BOX_SIDE_TOP * (ray.dy < 0);
            }
        }
    }

    if(ray.dz != 0){
        if(ray.dz > 0){
            d = (aabb.z - ray.z) / ray.dz; 
            z = aabb.z;
        }else if(ray.dz < 0){
            d = (aabb.z + aabb.depth - ray.z) / ray.dz; 
            z = aabb.z + aabb.depth;
        }
        x = ray.x + ray.dx * d;
        y = ray.y + ray.dy * d;
        
        int pred = (d >= 0 ) && 
            (y >= aabb.y) && (y < aabb.y + aabb.height) &&
            (x >= aabb.x) && (x < aabb.x + aabb.width);
        if(pred){
            if(!(*hit) || d < (*hitDistance)){
                *hitDistance = d;
                *hit = 1;
                *side = BOX_SIDE_FRONT * (ray.dz > 0) +  BOX_SIDE_BACK * (ray.dz < 0);
            }
        }
    }
}
void aabb_get_corners(Aabb aabb, vec3 corners[8]){
    corners[0][0] = aabb.x;
    corners[0][1] = aabb.y;
    corners[0][2] = aabb.z;

    corners[1][0] = aabb.x;
    corners[1][1] = aabb.y + aabb.height;
    corners[1][2] = aabb.z;

    corners[2][0] = aabb.x + aabb.width;
    corners[2][1] = aabb.y + aabb.height;
    corners[2][2] = aabb.z;

    corners[3][0] = aabb.x + aabb.width;
    corners[3][1] = aabb.y;
    corners[3][2] = aabb.z;

    corners[4][0] = aabb.x;
    corners[4][1] = aabb.y;
    corners[4][2] = aabb.z + aabb.depth;

    corners[5][0] = aabb.x;
    corners[5][1] = aabb.y + aabb.height;
    corners[5][2] = aabb.z + aabb.depth;

    corners[6][0] = aabb.x + aabb.width;
    corners[6][1] = aabb.y + aabb.height;
    corners[6][2] = aabb.z + aabb.depth;

    corners[7][0] = aabb.x + aabb.width;
    corners[7][1] = aabb.y;
    corners[7][2] = aabb.z + aabb.depth;

}
int aabb_inside_sphere(Aabb aabb, Sphere sphere){
    vec3 corners[8];
    int inCircleCount = 0;
    for(int i = 0; i < 8; i++){
        inCircleCount +=sphere_contains_point(sphere, 
            corners[i][0], corners[i][1], corners[i][2]);
    }
    return (inCircleCount == 8);
}

int aabb_intersects_sphere_d(Aabb aabb, Sphere sphere, float d, vec3 circleToAabb){
    float x, y, z;
    vec3 sphereCentre = {sphere.x, sphere.y, sphere.z};
    x = sphereCentre[0] + circleToAabb[0] * d;
    y = sphereCentre[1] + circleToAabb[1] * d;
    z = sphereCentre[2] + circleToAabb[2] * d;
    if(sphere_contains_point(sphere,x,y,z)){
        return 1;
    }
    return 0;
}
int aabb_intersects_sphere(Aabb aabb, Sphere sphere){
    vec3 circleToAabb;
    vec3 sphereCentre = {sphere.x, sphere.y, sphere.z};
    vec3 aabbCentre = {aabb.x + aabb.width * 0.5,
        aabb.y + aabb.height * 0.5,
        aabb.z + aabb.depth * 0.5
    };
    float x, y, z;
    vec3_sub(circleToAabb, aabbCentre, sphereCentre);
    float dleft = (aabb.x - sphere.x) / circleToAabb[0];
    if(aabb_intersects_sphere_d(aabb, sphere, dleft,  circleToAabb)){
        return 1;
    }
    float dright = (aabb.x + aabb.width - sphere.x) / circleToAabb[0];
    if(aabb_intersects_sphere_d(aabb, sphere, dright,  circleToAabb)){
        return 1;
    }
    float dbottom = (aabb.y - sphere.y) / circleToAabb[1];
    if(aabb_intersects_sphere_d(aabb, sphere, dbottom,  circleToAabb)){
        return 1;
    }
    float dtop = (aabb.y + aabb.height - sphere.y) / circleToAabb[1];
    if(aabb_intersects_sphere_d(aabb, sphere, dtop,  circleToAabb)){
        return 1;
    }
    float dfront = (aabb.z - sphere.z) / circleToAabb[2];
    if(aabb_intersects_sphere_d(aabb, sphere, dfront,  circleToAabb)){
        return 1;
    }
    float dback = (aabb.z + aabb.depth - sphere.z) / circleToAabb[2];
    if(aabb_intersects_sphere_d(aabb, sphere, dback,  circleToAabb)){
        return 1;
    }
    return 0;
}