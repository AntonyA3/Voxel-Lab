#ifndef VOXEL_TREE_H
#define VOXEL_TREE_H
#include "../include/linmath.h"
#include "../include/voxel.h"
#include "../include/aabb.h"
#include "../include/shape.h"


typedef struct 
{
    Voxel* head;
    int voxelCount;
}VoxelTree;
#endif
void voxel_tree_add(VoxelTree *voxelTree, int shape, void* shapeData);
void voxel_tree_delete(VoxelTree *voxelTree, int shape, void* shapeData);
void voxel_tree_editor(VoxelTree *voxelTree, int shape, void* shapeData, int action);