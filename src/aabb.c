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
