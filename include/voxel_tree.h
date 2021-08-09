#ifndef VOXEL_TREE_H
#define VOXEL_TREE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/voxel.h"
#include "../include/shape.h"

typedef struct {
    Voxel *head;
    int voxelCount;
    vec3 offset;
}VoxelTree;

void voxel_tree_contains_form(VoxelTree voxelTree, int shape, float *shapeData);
void voxel_tree_expander(VoxelTree *voxelTree, int shape, float *shapeData);
void voxel_tree_edit(VoxelTree *voxelTree, int shape, float *shapeData);

/*Voxel Blit*/
/*
void voxel_tree_cut_from_shape(VoxelTree *giver, VoxelTree *reciever, int shapeId, float *geometry);
void voxel_delete_voxels_from_shape(Voxel **voxel, int shapeId, float* geometry);
void voxel_add_voxels_from_shape(Voxel *voxel, int shapeId, float *geometry, int isNew);
void voxel_modify_from_tree(Voxel *secondaryTree, vec3 offset, VoxelTree *mainTree, int modification);

int shape_contains_point(int shapeId, float *geometry, float x, float y, float z);
void voxel_tree_expand_empty(VoxelTree *voxelTree, unsigned int x, unsigned int y, unsigned int z);
void voxel_tree_expand_head(VoxelTree *voxelTree, unsigned int x, unsigned int y, unsigned int z);


void voxel_tree_expand_empty_for_aabb(VoxelTree *voxelTree, Aabb aabb);
void voxel_tree_init_head(VoxelTree *voxelTree);
void voxel_tree_expand_head_for_aabb(VoxelTree *voxelTree, Aabb aabb);
void voxel_tree_expand_empty_for_sphere(VoxelTree *voxelTree, Sphere);
void voxel_tree_expand_head_for_sphere(VoxelTree *voxelTree, Sphere sphere);
void voxel_tree_double_empty(VoxelTree *voxelTree);
void voxel_tree_free_all_children(VoxelTree *voxelTree);
void voxel_tree_copy_from_shape(VoxelTree *giver, VoxelTree *reciever, int shapeId, float *geometry);
void voxel_tree_paste_from_voxel_tree(VoxelTree *giver, VoxelTree *reciever);
void voxel_tree_modify_from_voxel_tree(VoxelTree *giver, VoxelTree *reciever, int modification);
void voxel_paste_from_voxel_tree(Voxel *giver, vec3 offset, VoxelTree *reciever);

void voxel_tree_create_new_model_from_aabb_selected(VoxelTree *voxelTree, VoxelTree *selectionModel, Aabb aabb);
void voxel_tree_create_new_model_from_sphere_selected(VoxelTree *voxelTree, VoxelTree *selectionModel, Aabb aabb);
void voxel_tree_merge_model(VoxelTree *voxelTree, VoxelTree *childModel);

int voxel_tree_is_point_in_model(VoxelTree *voxelTree, float x, float y, float z);
int voxel_tree_count_voxels(VoxelTree *voxelTree);
void voxel_tree_pick_cell(int hitSide,int hitEntity,int editMode, vec3 hitpoint, float *cellX, float *cellY, float *cellZ);
int voxel_tree_delete_voxels_from_voxel_editor(VoxelTree *voxelTree, VoxelEditor *voxelEditor, vec3 hitpoint ,int hitEntity,int hitSide, float cellX, float cellY, float cellZ);
void voxel_tree_voxel_hit_correction(int side, int action , float *x, float *y, float *z);
int voxel_tree_edit_voxels_from_voxel_editor(VoxelTree *voxelTree, VoxelEditor *voxelEditor, int buttonState,  vec3 hitpoint, int hitentity, int hitside, float deltaTime);
int voxel_tree_add_voxels_from_voxel_editor(VoxelTree *voxelTree, VoxelEditor *voxelEditor, vec3 hitpoint ,int hitEntity,int hitSide,
    float cellX, float cellY, float cellZ);

*/
#endif
