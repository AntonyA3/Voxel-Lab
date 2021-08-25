#ifndef WORLD_H
#define WORLD_H
#include "../include/camera.h"
#include "../include/voxel_object.h"
#include "../include/color.h"
#include "../include/floor_grid.h"
enum world_object_id{WORLD_NO_OBJECT, WORLD_VOXEL_OBJECT};

typedef struct WorldEditResult{
    int changed;
}WorldEditResult;

typedef struct World{
    Camera viewCamera;
    ColorRGBAF bgColor;
    FloorGrid floorGrid;
    VoxelObject* voxelObject;
    int voxelObjectCount;

}World;


void world_add_voxel_object(World *world);

/*
void world_init(World *world);
void world_editor_init(WorldEditor* worldEditor);
void world_edit(World *world, WorldEditor worldEditor);


void world_update(World *world);
void world_re_model(World* world, WorldEditResult worldEditResult);
*/
#endif