#ifndef VOXEL_OBJECT_H
#define VOXEL_OBJECT_H
#include <stdlib.h>
#include "../include/voxel_octree.h"
#include "../include/sparce_voxel_octree.h"

/*
typedef struct
{
    SparceVoxelOctreeEditor sparceVoxelOctreeEditor;

}VoxelObjectEditor;
*/
typedef struct
{
    SparceVoxelOctree graphic;
    VoxelOctreeTree collider;
}VoxelObject;

void voxel_object_init(VoxelObject *voxelObject);
#endif