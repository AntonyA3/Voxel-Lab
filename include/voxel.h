#ifndef VOXEL_H
#define VOXEL_H
#include <stdlib.h>
#include "../include/aabb.h"
#include "../include/sphere.h"


typedef struct Voxel
{
    unsigned int size;
    int origin[3];
    struct Voxel *child[8];
}Voxel;

Aabb voxel_to_aabb(Voxel voxel);
void voxel_init_children_null(Voxel *voxel);
int voxel_count_voxels(Voxel* voxel);
/*
Aabb voxel_to_aabb(Voxel voxel);
Aabb voxel_to_aabb_from_child_i(Voxel voxel, int i);
void voxel_init_children_as_null(Voxel *voxel);
void voxel_add_voxel(Voxel *voxel, unsigned int x, unsigned int y, unsigned int z);
void voxel_delete_voxel(Voxel **voxel, unsigned int x, unsigned int y, unsigned int z);
void voxel_delete_voxel_from_aabb(Voxel **voxel, Aabb aabb);
void voxel_delete_voxel_from_sphere(Voxel **voxel, Sphere sphere);
void voxel_generate_model(Voxel *voxel, ColorRgbaF color, PosColor32Vertex *vertexArray, unsigned int *elementArray, int *index);
void voxel_do_merge_voxels(Voxel *voxel);
void voxel_add_voxels_from_aabb(Voxel *voxel, Aabb aabb, int isEmpty);
void voxel_add_voxels_from_sphere(Voxel *voxel, Sphere sphere, int isEmpty);
void voxel_free_children(Voxel *voxel);
void voxel_get_from_aabb(Voxel *voxel);
void voxel_regenerate_children(Voxel *voxel);

int voxel_index_is_left(int index);
int voxel_index_is_bottom(int index);
int voxel_index_is_front(int index);
int voxel_count_voxels(Voxel *voxel);
int voxel_is_leaf(Voxel *voxel);
int voxel_is_point_in_voxel(Voxel* voxel,float x, float y,float z);
int voxel_count_voxels_in_aabb(Voxel *voxel, Aabb aabb);

Voxel* voxel_create_size_1_voxel(unsigned int x, unsigned int y, unsigned int z);
Voxel* voxel_create_size_gte_2_voxel(Voxel *parent, int isLeft, int isBottom, int isFront);
*/
#endif