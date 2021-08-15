#include "../include/aabb.h"

float AABB_VERTEX_ARRAY[FLOATS_PER_CUBE] = {
    //left
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    //top
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    //front    
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    //bottom
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    //right
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    //back
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f
};

float AABB_NORMAL_ARRAY[FLOATS_PER_CUBE] = {
    //left
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    //top
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    //front
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    //bottom
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    //right
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    //back
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
};

unsigned AABB_ELEMENTS_ARRAY[ELEMENTS_PER_CUBE] = {
    0, 1, 2,        0, 2, 3,
    4, 5, 6,        4, 6, 7,
    8, 9, 10,       8, 10, 11,
    12, 13, 14,     12, 14, 15,
    16, 17, 18,     16, 18, 19,
    20, 21, 22,     20, 22, 23
};

int aabb_intersects_aabb(Aabb aabb1, Aabb aabb2){
    float totalWidth = aabb1.width + aabb2.width;
    float totalHeight = aabb1.height + aabb2.height;
    float totalDepth = aabb1.depth + aabb2.depth;
    float minX = fminf(aabb1.x, aabb2.x);
    float maxX = fmaxf(aabb1.x + aabb1.width, aabb2.x + aabb2.width);
    float minY = fminf(aabb1.y, aabb2.y);
    float maxY = fmaxf(aabb1.y + aabb1.height, aabb2.y + aabb2.height);
    float minZ = fminf(aabb1.z, aabb2.z);
    float maxZ = fmaxf(aabb1.z + aabb1.depth, aabb2.z + aabb2.depth);
    return ((totalWidth - (maxX - minX)) >= 0) &&
        ((totalHeight - (maxY - minY)) >= 0) &&
        ((totalDepth - (maxZ - minZ)) >= 0);
}
int aabb_contains_point(Aabb aabb, float x, float y, float z){
    return (x >= aabb.x) && (x <= (aabb.x + aabb.width)) &&
        (y >= aabb.y) && (y <= (aabb.y + aabb.height)) &&
        (z >= aabb.z) && (z <= (aabb.z + aabb.depth));
}

Aabb aabb_from_origin_halfExtents(vec3 origin, vec3 halfExtents){
    Aabb aabb;
    vec3_sub(aabb.min, origin, halfExtents);
    vec3_scale(aabb.size, halfExtents, 2.0);
    return aabb;
}
Aabb aabb_from_expand(Aabb aabb, float expansion){
    float expanse2 = expansion * 2;
    Aabb aabb1 ={
        aabb.x - expansion,
        aabb.y - expansion,
        aabb.z - expansion,
        aabb.width + expansion,
        aabb.height + expansion,
        aabb.depth + expansion
    };
    return aabb1;
}

void aabb_get_corners(Aabb aabb, vec3 corners[8]){
    for(int i = 0; i < 8; i++){
        memcpy(&corners[i], aabb.min, sizeof(vec3));
    }
    corners[1][1] += aabb.height;

    corners[2][0] += aabb.width;
    corners[2][1] += aabb.height;

    corners[3][0] += aabb.width;

    corners[4][2] += aabb.depth;

    corners[5][1] += aabb.height;
    corners[5][2] += aabb.depth;

    corners[6][0] += aabb.width;
    corners[6][1] += aabb.height;
    corners[6][2] += aabb.depth;

    corners[7][0] += aabb.width;
    corners[7][2] += aabb.depth;    
}

void aabb_get_origin(Aabb aabb, vec3 origin){
    vec3 halfExtents;
    vec3_scale(halfExtents, aabb.size, 0.5);
    vec3_add(origin, aabb.min, halfExtents);
}