#ifndef VOXEL_EDITOR_H
#define VOXEL_EDITOR_H
#include "../include/aabb.h"
#include "../include/sphere.h"
typedef struct{
    int dynamics, editMode;
    int shape;
    float flowTick, flowRate;
    Aabb aabb;
    Sphere sphere;
}VoxelEditor;
#endif