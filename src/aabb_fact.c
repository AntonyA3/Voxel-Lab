#include "../include/aabb_fact.h"


Aabb voxel_to_aabb(Voxel voxel){
    Aabb aabb;
    float halfSize = voxel.size * 0.5;
    int pred = halfSize *(voxel.size!=1);
    aabb.x = voxel.origin[0] - pred;
    aabb.y = voxel.origin[1] - pred;
    aabb.z = voxel.origin[2] - pred;
    aabb.width = voxel.size;
    aabb.height = voxel.size;
    aabb.depth = voxel.size;
    return aabb;
}