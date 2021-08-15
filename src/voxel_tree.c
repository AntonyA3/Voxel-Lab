#include "../include/voxel_tree.h"

int voxel_tree_child_from_trajectory(vec3 trajectory){
    switch (
        1 * (trajectory[0] >= 0) + 
        2 * (trajectory[1] >= 0) +
        4 * (trajectory[2] >= 0)
    ){
    case 0:
        return 6;
    case 1:
        return 5;
    case 2:
        return 7;
    case 3:
        return 4;
    case 4:
        return 2;
    case 5:
        return 1;
    case 6:
        return 3;
    case 7:
        return 0;
    }
}

void voxel_tree_add(VoxelTree *voxelTree, int shape, void* shapeData){
    int wasEmpty = 0;
    if(voxelTree->head == NULL){
        wasEmpty = 1;
        voxelTree->head = (Voxel*) malloc(sizeof(Voxel));
        voxelTree->head->origin[0] = 0;
        voxelTree->head->origin[1] = 0;
        voxelTree->head->origin[2] = 0;
        voxelTree->head->size = 2;
        voxel_init_children_to_null(voxelTree->head);
  
    }

    switch (wasEmpty)
    {
    case 1:
        {
            Aabb voxBox = voxel_to_aabb(*voxelTree->head);
            Aabb shapeAabb = shape_to_aabb(shape, shapeData);
            vec3 corners[8];
            aabb_get_corners(shapeAabb, corners);

            for(int i = 0; i < 8; i++){
                int n = 0;
                while((n < 100) && !aabb_contains_point(voxBox, corners[i][0], corners[i][1], corners[i][2])){
                    voxelTree->head->size *= 2;
                    voxBox = voxel_to_aabb(*voxelTree->head);
                    n++;
                }
            }
        }
        break;
    case 0:
        {
            Aabb voxBox = voxel_to_aabb(*voxelTree->head);
            Aabb shapeAabb = shape_to_aabb(shape, shapeData);
            vec3 corners[8];
            aabb_get_corners(shapeAabb, corners);
            for(int i = 0; i < 8; i++){
            }
            for(int i = 0; i < 8; i++){
                vec3 trajectory;
                vec3_sub(trajectory, corners[i], voxBox.min);
                vec3 growTraj = {
                    (trajectory[0] >= 0) * 1 + (trajectory[0] < 0) * -1,
                    (trajectory[1] >= 0) * 1 + (trajectory[1] < 0) * -1,
                    (trajectory[2] >= 0) * 1 + (trajectory[2] < 0) * -1,
                };
                int childNumber = voxel_tree_child_from_trajectory(trajectory);
                int n = 0;
                while ((n < 100) && !aabb_contains_point(voxBox, corners[i][0], corners[i][1], corners[i][2]))
                {
                    Voxel* newHead = (Voxel*) malloc(sizeof(Voxel));
                    newHead->size = voxelTree->head->size * 2;
                    int hs = voxelTree->head->size / 2;
                    newHead->origin[0] = voxelTree->head->origin[0] + hs * growTraj[0];
                    newHead->origin[1] = voxelTree->head->origin[1] + hs * growTraj[1];
                    newHead->origin[2] = voxelTree->head->origin[2] + hs * growTraj[2];
                    for(int i = 0; i < 8; i++){
                        newHead->child[i] = NULL;
                    }
                    newHead->child[childNumber] = voxelTree->head;
                    voxelTree->head = newHead;
                    voxBox = voxel_to_aabb(*voxelTree->head);
                    n++;
                }                
            }
        }
        break;
    }
    voxel_edit_voxel_df(&voxelTree->head, shape, shapeData, VOXEL_ACTION_ADD);
}

void voxel_tree_delete(VoxelTree *voxelTree, int shape, void* shapeData){
    if(voxelTree->head != NULL){
        voxel_edit_voxel_df(&voxelTree->head, shape, shapeData, VOXEL_ACTION_DELETE);
    }
}

void voxel_tree_editor(VoxelTree *voxelTree, int shape, void* shapeData, int action){
    switch (action)
    {
    case VOXEL_ACTION_ADD:
        
        voxel_tree_add(voxelTree, shape, shapeData);
        break;
    
    case VOXEL_ACTION_DELETE:
        voxel_tree_delete(voxelTree, shape, shapeData);
        break;
    }
    
}