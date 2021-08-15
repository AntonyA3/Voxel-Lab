#include "../include/shape.h"

int shape_intersects_aabb(int shape, void* shapeData, Aabb aabb){
    switch (shape)
    {
    case SHAPE_AABB:
        {
        Aabb aabb1;
        memcpy(&aabb1, shapeData, sizeof(Aabb));
        return aabb_intersects_aabb(aabb, aabb1);
        }
    case SHAPE_SPHERE:
        {
            Sphere sphere;
            memcpy(&sphere, shapeData, sizeof(Sphere));
            return sphere_intersects_aabb(sphere, aabb);
        }
    }
    return 0;
}

int shape_contains_point(int shape, void* shapeData, float x, float y, float z){
    switch (shape)
    {
    case SHAPE_AABB:
        {            
            Aabb aabb;
            memcpy(&aabb, shapeData, sizeof(Aabb));
            return aabb_contains_point(aabb, x, y, z);
        }
    case SHAPE_SPHERE:
        return 0;
    }
    return 0;
}

Aabb shape_to_aabb(int shape, void* shapeData){
    switch (shape)
    {
    case SHAPE_AABB:
        {    
            Aabb aabb;
            memcpy(&aabb, shapeData, sizeof(Aabb));
            return aabb;
        }
        break;      
    case SHAPE_SPHERE:
        {
            Sphere sphere;
            memcpy(&sphere, shapeData, sizeof(Sphere));
            return sphere_to_aabb(sphere);
        }
        break;
    }
}