#include "../include/voxel.h"
/*
int voxel_i_is_left(int i){
    return (i == 0) || (i == 1) || (i == 4) || (i == 5);
}
int voxel_i_is_bottom(int i){
    return (i == 0) || (i == 3) || (i == 4) || (i == 7);
}
int voxel_i_is_front(int i){
    return (i == 0) || (i == 1) || (i == 2) || (i == 3);
}
void voxel_init_children_to_null(Voxel* voxel){
    for(int i = 0; i < VOXEL_CHILD_COUNT; i++){
        voxel->child[i] = NULL;

    }
}
Aabb voxel_child_to_aabb(Voxel voxel, int childId){
    float hs = voxel.size / 2;
    Aabb aabb = {voxel.origin[0], voxel.origin[1], voxel.origin[2],
        hs, hs, hs
    };
    int isLeft = voxel_i_is_left(childId);
    int isBottom = voxel_i_is_bottom(childId);
    int isFront = voxel_i_is_front(childId);

    aabb.x -= isLeft * hs;
    aabb.y -= isBottom * hs;
    aabb.z -= isFront * hs;
    return aabb; 
}

Aabb voxel_to_aabb(Voxel voxel){
    switch (voxel.size)
    {
    case 1:
        {
            Aabb aabb = {{
                voxel.origin[0], voxel.origin[1], 
                voxel.origin[2],1 ,1 ,1
            }};
            return aabb;
        }
        break;  
    default:
        {
            float hs = voxel.size / 2;
            Aabb aabb = {{
                voxel.origin[0] - hs, voxel.origin[1] - hs, voxel.origin[2] - hs,
                voxel.size, voxel.size, voxel.size
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
        for(int i = 0; i < 8; i++){
            int nullcount = 0;
            nullcount += voxel->child[i] == NULL;
            return (nullcount == 8);
            
        }
    }
}
int voxel_count(Voxel *voxel){

    switch (voxel->size)
    {
    case 1:
        return 1;    
    default:
        switch (voxel_is_leaf(voxel))
        {
        case 1:
            return 1;
        default:
            {
                int total = 0;
                for(int i = 0; i < 8; i++){
                    if(voxel->child[i] != NULL){
                        total += voxel_count(voxel->child[i]);
                    }
                }
                return total;
            }
        }
    }
}



void voxel_split(Voxel *voxel){
    for(int i = 0; i < VOXEL_CHILD_COUNT; i++){
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
void voxel_edit_voxel_df(Voxel **voxel, int shape, void *shapeData, int action, int *changed){

    switch ((*voxel)->size)
    {
    case 2:
        for(int i = 0; i < VOXEL_CHILD_COUNT; i++){
            Aabb childBox = voxel_child_to_aabb(**voxel, i);
            if(shape_contains_point(shape, shapeData, 
                childBox.x + VOXEL_HALF_SIZE,
                childBox.y + VOXEL_HALF_SIZE,
                childBox.z + VOXEL_HALF_SIZE)
            ){
                switch (action)
                {
                case VOXEL_ACTION_ADD:
                    if((*voxel)->child[i] == NULL){
                        *changed = 1;
                        (*voxel)->child[i] = (Voxel*) malloc(sizeof(Voxel));
                        (*voxel)->child[i]->size = (*voxel)->size / 2;
                        (*voxel)->child[i]->origin[0] = childBox.x;
                        (*voxel)->child[i]->origin[1] = childBox.y;
                        (*voxel)->child[i]->origin[2] = childBox.z;
                        voxel_init_children_to_null((*voxel)->child[i]);
                    }
                    break;
                
                case VOXEL_ACTION_DELETE:
                    if((*voxel)->child[i] != NULL){
                        *changed = 1;
                        free((*voxel)->child[i]);
                        (*voxel)->child[i] = NULL;
                    }
                    break;
                }
            }
        }
        break;
    default:
        if(action == VOXEL_ACTION_DELETE){
            if(voxel_is_leaf(*voxel)){
                voxel_split(*voxel);
            }
        }
        for(int i = 0; i < VOXEL_CHILD_COUNT; i++){
            vec3 midpoint;
            Aabb voxBox = voxel_child_to_aabb(**voxel, i);
            Aabb predBox = aabb_from_expand(voxBox, -0.5);
            aabb_get_origin(voxBox, midpoint);
            

            if(shape_intersects_aabb(shape, shapeData, predBox)){
                switch (action)
                {
                case VOXEL_ACTION_ADD:
                    if((*voxel)->child[i] == NULL){
                        (*voxel)->child[i] = (Voxel*) malloc(sizeof(Voxel));
                        (*voxel)->child[i]->size = (*voxel)->size / 2;
                        (*voxel)->child[i]->origin[0] = midpoint[0];
                        (*voxel)->child[i]->origin[1] = midpoint[1];
                        (*voxel)->child[i]->origin[2] = midpoint[2];
                        voxel_init_children_to_null((*voxel)->child[i]);
                    }
                    voxel_edit_voxel_df(&(*voxel)->child[i], shape, shapeData, VOXEL_ACTION_ADD, changed);
                    break;
                
                case VOXEL_ACTION_DELETE:
                    if((*voxel)->child[i] != NULL){
                        voxel_edit_voxel_df(&(*voxel)->child[i], shape, shapeData, VOXEL_ACTION_DELETE, changed); 
                    }
                    break;
                }
            }     
        }
        break;
    }
}
*/

