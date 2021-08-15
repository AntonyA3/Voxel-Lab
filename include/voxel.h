#ifndef VOXEL_H
#define VOXEL_H
#define VOXEL_CHILD_COUNT 8
#define VOXEL_HALF_SIZE 0.5
#include "../include/aabb.h"
#include "../include/shape.h"

enum voxel_action{VOXEL_ACTION_ADD, VOXEL_ACTION_DELETE};

typedef struct Voxel
{
    int origin[3];
    unsigned int size;
    struct Voxel *child[8];
}Voxel;

Aabb voxel_to_aabb(Voxel voxel);
void voxel_delete_voxel(Voxel **voxel, int shape, void *shapeData);
void voxel_add_voxel(Voxel *voxel, int shape, void *shapeData);
void voxel_edit_voxel_df(Voxel **voxel, int shape, void *shapeData, int action);
void voxel_init_children_to_null(Voxel* voxel);
int voxel_count(Voxel *voxel);
int voxel_is_leaf(Voxel *voxel);
#endif