#include "../include/aabb.h"
void aabb_get_centre(Aabb aabb, vec3 centre){
    centre[0] = aabb.x + aabb.w * 0.5f;
    centre[1] = aabb.y + aabb.h * 0.5f;
    centre[2] = aabb.z + aabb.d * 0.5f;
}
void aabb_get_max_corner(Aabb aabb, vec3 point){
    point[0] = aabb.x + aabb.w;
    point[1] = aabb.y + aabb.h;
    point[2] = aabb.z + aabb.d;
}

int aabb_contains_point(Aabb aabb, float x, float y, float z){
    return (x >= aabb.x) && (y >= aabb.y) && (z >= aabb.z) &&
    (x < (aabb.x + aabb.w)) && (y < (aabb.y + aabb.h)) && (z < (aabb.z + aabb.d));
}

int aabb_contains_point_inclusive(Aabb aabb, float x, float y, float z){
    return (x >= aabb.x) && (y >= aabb.y) && (z >= aabb.z) &&
    (x <= (aabb.x + aabb.w)) && (y <= (aabb.y + aabb.h)) && (z <= (aabb.z + aabb.d));
}

int aabb_contains_aabb(Aabb aabbContainer, Aabb aabb){
    return  aabb_contains_point(aabbContainer, aabb.x, aabb.y, aabb.z) &&
    aabb_contains_point_inclusive(aabbContainer, aabb.x + aabb.w, aabb.y + aabb.h, aabb.z + aabb.d);
}

void aabb_get_overlaping_area(Aabb aabb1, Aabb aabb2, vec3 overlap){

    float overlapX = (aabb1.w + aabb2.w) -(
        maxf(aabb1.x + aabb1.w, aabb2.x + aabb2.w) - 
        minf(aabb1.x, aabb2.x))
    ;
    float overlapY = (aabb1.h + aabb2.h) - (
        maxf(aabb1.y + aabb1.h, aabb2.y + aabb2.h) - 
        minf(aabb1.y, aabb2.y)
    );

    float overlapZ = (aabb1.d + aabb2.d) -(
        maxf(aabb1.z + aabb1.d, aabb2.z + aabb2.d) - 
        minf(aabb1.z, aabb2.z)
    );

    overlap[0] = maxf(0, overlapX);
    overlap[1] = maxf(0, overlapY);
    overlap[2] = maxf(0, overlapZ);
    int overlapping = (overlap[0] * overlap[1] * overlap[2]) != 0;
    vec3_scale(overlap, overlap, overlapping);
  
}

Aabb aabb_get_box_with_subtracted_corners(Aabb aabb, float subtraction){
    aabb.x += subtraction;
    aabb.y += subtraction;
    aabb.z += subtraction;
    float sub2 = subtraction * 2;
    aabb.w -= sub2;
    aabb.h -= sub2;
    aabb.d -= sub2;

    return aabb;
}

Aabb aabb_from_origin_halfExtents(vec3 origin, vec3 halfExtents){
    Aabb box;
    vec3_sub(box.min, origin, halfExtents);
    vec3_scale(box.extents, halfExtents, 2.0);

    return box;
}


void aabb_get_corner(Aabb aabb, int corner, vec3 result){
    memcpy(&result, aabb.min, sizeof(vec3));
    switch (corner)
    {
    case 0:
        break;
    case 1:
        result[1] += aabb.h;
        break;
    case 2:
        result[0] +=  aabb.w;
        result[1] +=  aabb.h;
        break;
    case 3:
        result[0] +=  aabb.w;
        break;
    case 4:
        result[2] +=  aabb.d;
        break;
    case 5:
        result[1] +=  aabb.h;
        result[2] +=  aabb.d;
        break;
    case 6:
        result[0] += aabb.w;
        result[1] += aabb.h;
        result[2] += aabb.d;
        break;
    case 7:
        result[0] += aabb.w;
        result[2] += aabb.d;
        break;
    }
}

void aabb_get_box_corners(Aabb aabb, vec3 corners[8]){
    float aabbCorner[8][3] = {
        {aabb.x, aabb.y, aabb.z},
        {aabb.x, aabb.y + aabb.h, aabb.z},
        {aabb.x + aabb.w, aabb.y + aabb.h, aabb.z},
        {aabb.x +  aabb.w, aabb.y, aabb.z},
        {aabb.x, aabb.y, aabb.z + aabb.d},
        {aabb.x, aabb.y + aabb.h, aabb.z + aabb.d},
        {aabb.x + aabb.w, aabb.y + aabb.h, aabb.z + aabb.d},
        {aabb.x +  aabb.w, aabb.y, aabb.z + aabb.d},
    };


    memcpy(corners, aabbCorner, sizeof(vec3) * 8);
}

