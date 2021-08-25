#ifndef VOXEL_TREE_H
#define VOXEL_TREE_H
#include "../include/linmath.h"
#include "../include/shape.h"

enum voxel_action{VOXEL_ACTION_ADD, VOXEL_ACTION_DELETE};

typedef struct VoxelOctreeEditor{
    int action;
    int shapeType;
    int changed;
    void* ShapeData

}VoxelOctreeEditor;

typedef struct Voxel
{
    int size;
    int origin[3];
    struct Voxel *child[8];
}Voxel;

typedef struct 
{
    Voxel* head;
    int voxelCount;
}VoxelOctreeTree;

int voxel_index_is_left(int index);
int voxel_index_is_bottom(int index);
int voxel_index_is_front(int index);
void voxel_init_children_to_null(Voxel* voxel);
Aabb voxel_to_aabb(Voxel *voxel);
int voxel_is_leaf(Voxel *voxel);
void voxel_octree_expand_empty(VoxelOctreeTree *voxelOctree, VoxelOctreeEditor *editor);
void voxel_octree_expand_head(VoxelOctreeTree *voxelOctree, VoxelOctreeEditor *editor);
Aabb voxel_child_to_aabb(Voxel voxel, int childId);
void voxel_edit_voxel(Voxel **voxel, VoxelOctreeEditor *editor);
void voxel_split(Voxel *voxel);
void voxel_octree_add(VoxelOctreeTree *voxelTree,  VoxelOctreeEditor *editor);
void voxel_octree_delete(VoxelOctreeTree *voxelTree, int shape, void* shapeData, int *changed);
void voxel_octree_edit(VoxelOctreeTree *voxelTree, VoxelOctreeEditor *editor);

/*
int voxel_index_is_left(int index);
int voxel_index_is_bottom(int index);
int voxel_index_is_front(int index);
void voxel_split(Voxel *voxel);
void voxel_init_children_to_null(Voxel* voxel);
int voxel_is_leaf(Voxel *voxel);
void voxel_octree_add(VoxelOctreeTree *voxelTree,  VoxelOctreeEditor *editor);
Aabb voxel_to_aabb(Voxel voxel);
*/
#endif
/*
void voxel_tree_delete(VoxelTree *voxelTree, int shape, void* shapeData, int *changed);
//void voxel_tree_edit(VoxelTree *voxelTree, int shape, void* shapeData, int action, int *changed);
void voxel_tree_edit(VoxelTree *voxelTree, VoxelTreeEditor voxelTreeEditor);

int voxel_tree_count(VoxelTree *voxelTree);
*/
