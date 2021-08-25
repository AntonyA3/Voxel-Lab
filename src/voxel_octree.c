#include "../include//voxel_octree.h"


int voxel_index_is_left(int index){
    return (index == 0) || (index == 2) || (index == 4) || (index == 6);
}

int voxel_index_is_bottom(int index){
    return (index == 0) || (index == 1) || (index == 4) || (index == 5);
}

int voxel_index_is_front(int index){
    return (index == 0) || (index == 1) || (index == 2) || (index == 3);
}

void voxel_init_children_to_null(Voxel* voxel){
    for(int i = 0; i < 8; i++){
        voxel->child[i] = NULL;
    }
}

Aabb voxel_to_aabb(Voxel *voxel){
    switch (voxel->size)
    {
    case 1:
        {
            Aabb aabb = {{
                voxel->origin[0], voxel->origin[1], 
                voxel->origin[2],1 ,1 ,1
            }};
            return aabb;
        }
        break;  
    default:
        {
            float hs = voxel->size / 2;
            float size = voxel->size;
            Aabb aabb = {{
                voxel->origin[0] - hs, voxel->origin[1] - hs, voxel->origin[2] - hs,
                size, size, size
            }};
            return aabb;
        }
        break;
    }
}

int voxel_is_leaf(Voxel *voxel){
    switch (voxel->size)
    {
    case 1:
        return 1;
    default:
        {
            int nullcount = 0;
            for(int i = 0; i < 8; i++){
                nullcount += voxel->child[i] == NULL;
            }            
            return (nullcount == 8);
        }

    }
}

void voxel_octree_expand_empty(VoxelOctreeTree *voxelOctree, VoxelOctreeEditor *editor){   
    Aabb voxBox = voxel_to_aabb(voxelOctree->head);
    Aabb shapeAabb = shape_to_aabb(editor->shapeType, editor->ShapeData);
    vec3 corners[8];
    aabb_get_corners(shapeAabb, corners);
    for(int i = 0; i < 8; i++){
        while(!aabb_contains_point(voxBox, corners[i][0], corners[i][1], corners[i][2])){
            voxelOctree->head->size *= 2;
            voxBox = voxel_to_aabb(voxelOctree->head);
        }
    }
}


void voxel_octree_expand_head(VoxelOctreeTree *voxelOctree, VoxelOctreeEditor *editor){
    Aabb voxBox = voxel_to_aabb(voxelOctree->head);
    Aabb shapeAabb = shape_to_aabb(editor->shapeType, editor->ShapeData);
    vec3 corners[8];
    aabb_get_corners(shapeAabb, corners);
    
    for(int i = 0; i < 8; i++){
      vec3 vectorToPoint, trajectory;
        vec3_sub(vectorToPoint, corners[i], voxBox.min);
        trajectory[0] = (vectorToPoint[0] >= 0) * 1 + (vectorToPoint[0] < 0) * -1;
        trajectory[1] = (vectorToPoint[1] >= 0) * 1 + (vectorToPoint[1] < 0) * -1;
        trajectory[2] = (vectorToPoint[2] >= 0) * 1 + (vectorToPoint[2] < 0) * -1;
  
        int childNumber = 7;
        childNumber -= (trajectory[0] == 1);
        childNumber -= 2 *(trajectory[1] == 1);
        childNumber -= 4 *(trajectory[2] == 1);
        while (!aabb_contains_point(voxBox, corners[i][0], corners[i][1], corners[i][2]))
        {
            Voxel* newHead = (Voxel*) malloc(sizeof(Voxel));
            Voxel* oldHead = (Voxel*) voxelOctree->head;

            newHead->size = oldHead->size * 2;
            int hs = oldHead->size / 2;
            newHead->origin[0] = oldHead->origin[0] + hs * trajectory[0];
            newHead->origin[1] = oldHead->origin[1] + hs * trajectory[1];
            newHead->origin[2] = oldHead->origin[2] + hs * trajectory[2];
    
            voxel_init_children_to_null(newHead);
            newHead->child[childNumber] = oldHead;
            voxelOctree->head = newHead;
            voxBox = voxel_to_aabb(newHead);           
        } 
    }
}

Aabb voxel_child_to_aabb(Voxel voxel, int childId){
    float hs = voxel.size / 2;
    Aabb aabb = {voxel.origin[0], voxel.origin[1], voxel.origin[2],
        hs, hs, hs
    };
    int isLeft = voxel_index_is_left(childId);
    int isBottom = voxel_index_is_bottom(childId);
    int isFront = voxel_index_is_front(childId);

    aabb.x -= isLeft * hs;
    aabb.y -= isBottom * hs;
    aabb.z -= isFront * hs;
    return aabb; 
}


void voxel_edit_voxel(Voxel **voxel, VoxelOctreeEditor *editor){
    switch ((*voxel)->size)
    {
    case 2:
        for(int i = 0; i < 8; i++){
            Aabb childBox = voxel_child_to_aabb(**voxel, i);
            float px = childBox.x + 0.5;
            float py = childBox.y + 0.5;
            float pz = childBox.z + 0.5;
            if(shape_contains_point(editor->shapeType, editor->ShapeData, px, py, pz)){
                if((*voxel)->child[i] == NULL){
                    editor->changed = 1;
                    (*voxel)->child[i] = (Voxel*) malloc(sizeof(Voxel));
                    (*voxel)->child[i]->size = (*voxel)->size / 2;
                    (*voxel)->child[i]->origin[0] = (int)childBox.x;
                    (*voxel)->child[i]->origin[1] = (int)childBox.y;
                    (*voxel)->child[i]->origin[2] = (int)childBox.z;
                    voxel_init_children_to_null((*voxel)->child[i]);
                }
            }
        }
        int childCount = 0;
        for(int i = 0; i < 8; i++){
            childCount += (*voxel)->child[i] != NULL;
        }
        printf("childcount is %d\n", childCount);
        if(childCount == 8){
            for(int i = 0;i < 8; i++){
                free((*voxel)->child[i]);
                (*voxel)->child[i] = NULL;
            }
        }
        break;
    default:
        for(int i = 0; i < 8; i++){
            vec3 midpoint;
            Aabb voxBox = voxel_child_to_aabb(**voxel, i);
            Aabb predBox = aabb_from_expand(voxBox, -0.5);
            aabb_get_origin(voxBox, midpoint);
            if(shape_intersects_aabb(editor->shapeType, editor->ShapeData, predBox)){
                if((*voxel)->child[i] == NULL){
                    (*voxel)->child[i] = (Voxel*) malloc(sizeof(Voxel));
                    (*voxel)->child[i]->size = (*voxel)->size / 2;
                    (*voxel)->child[i]->origin[0] = midpoint[0];
                    (*voxel)->child[i]->origin[1] = midpoint[1];
                    (*voxel)->child[i]->origin[2] = midpoint[2];
                    voxel_init_children_to_null((*voxel)->child[i]);
                }
                voxel_edit_voxel(&(*voxel)->child[i], editor);
            }     
        }
        break;
    }
}

void voxel_split(Voxel *voxel){
    for(int i = 0; i < 8; i++){
        Aabb aabb;
        vec3 midpoint;
        voxel_child_to_aabb(*voxel, i);
        aabb_get_origin(aabb, midpoint);
        voxel->child[i] = (Voxel*) malloc(sizeof(Voxel));
        voxel->child[i]->origin[0] = midpoint[0];
        voxel->child[i]->origin[1] = midpoint[1];
        voxel->child[i]->origin[2] = midpoint[2];
        voxel_init_children_to_null(voxel->child[i]);
    }
}

void voxel_octree_add(VoxelOctreeTree *voxelTree, VoxelOctreeEditor *editor){
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
        voxel_octree_expand_empty(voxelTree, editor);
        break;
    case 0:
        voxel_octree_expand_head(voxelTree, editor);
        break;
    }
    voxel_edit_voxel(&voxelTree->head, editor);
}

void voxel_octree_edit(VoxelOctreeTree *voxelTree, VoxelOctreeEditor *editor){
    switch (editor->action)
    {
    case VOXEL_ACTION_ADD:
        voxel_octree_add(voxelTree, editor);
        break;
    case VOXEL_ACTION_DELETE:
        //voxel_tree_delete(voxelTree, editor);
        break;
    }
}
