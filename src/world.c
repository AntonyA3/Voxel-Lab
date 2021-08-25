#include "../include/world.h"

void world_add_voxel_object(World *world){
    world->voxelObjectCount += 1;
    world->voxelObject = (VoxelObject*) realloc(
        world->voxelObject,
        world->voxelObjectCount * sizeof(VoxelObject)
    );
    world->voxelObject[world->voxelObjectCount -1].graphic.head = NULL;
    world->voxelObject[world->voxelObjectCount -1].collider.head = NULL;
}
