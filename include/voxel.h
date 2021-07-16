
#ifndef VOXEL_H
#define VOXEL_H
#include <stdlib.h>
#include "../include/vector3ui.h"
#include "../include/ray.h"
#include "../include/aabb.h"
#include "../include/vector3.h"
#include "../include/entity_type.h"

typedef struct Voxel
{
    unsigned int size;
    unsigned int origin[3];
    struct Voxel* child[8];
}Voxel;
void voxel_set_children_to_null(Voxel* voxel);
int voxel_is_leaf(Voxel *voxel);
int voxel_contains_point(Voxel voxel, unsigned int x, unsigned int y, unsigned int z);
void voxel_add_voxel(Voxel** voxel, unsigned int x, unsigned int y, unsigned int z);
int voxel_count_voxels(Voxel* voxel);
void voxel_get_leaf_voxels(Voxel *voxel, Voxel **voxelArray, int *index);
void voxel_get_verticies_and_indicies(Voxel* voxel, float *vertexArray ,unsigned int *elementArray);
void voxel_generate_cube(Voxel *voxel,int index, float *verts, unsigned int *elements);
void voxel_vs_ray (Voxel *voxel, Ray ray, int *entityType, int *hit, int *side, float *hitDistance);
int voxel_is_point_inside_model(Voxel *voxel, float x, float y, float z);
#endif