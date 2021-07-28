#include "../include/voxel.h"

Aabb voxel_to_aabb(Voxel voxel){
    Aabb voxBox;
    if(voxel.size == 1){
        voxBox.x = voxel.origin[0];
        voxBox.y = voxel.origin[1];
        voxBox.z = voxel.origin[2];
        voxBox.w = 1.0;
        voxBox.h = 1.0;
        voxBox.d = 1.0;
    }else{
        int hs = voxel.size / 2;
        voxBox.x = voxel.origin[0] - hs;
        voxBox.y = voxel.origin[1] - hs;
        voxBox.z = voxel.origin[2] - hs;
        voxBox.w = voxel.size;
        voxBox.h = voxel.size;
        voxBox.d = voxel.size;
    }
    return voxBox;
}

void voxel_init_children_as_null(Voxel *voxel){
    for(int i = 0; i < 8; i++){
        voxel->child[i] = NULL;
    }   
}
int voxel_index_is_left(int i){
    return (i < 4);
}

int voxel_index_is_bottom(int i){
    return (i == 0) || (i == 1) || (i == 4) || (i == 5);
}

int voxel_index_is_front(int i){
    return (i == 0) || (i == 2) || (i == 4) || (i == 6);
}

Voxel* voxel_create_size_1_voxel(unsigned int x, unsigned int y, unsigned int z){
    Voxel* voxelChild = (Voxel*)malloc(sizeof(Voxel));
    voxelChild->size = 1;
    voxelChild->origin[0] = x;
    voxelChild->origin[1] = y;
    voxelChild->origin[2] = z;
    voxel_init_children_as_null(voxelChild);
    return voxelChild;
}

Voxel* voxel_create_size_gte_2_voxel(Voxel *parent,
    int isLeft, int isBottom, int isFront){
        Voxel* voxelChild = (Voxel*) malloc(sizeof(Voxel));
        voxelChild->size = parent->size / 2;
        int qs = parent->size / 4;
        voxelChild->origin[0] = parent->origin[0] + (isLeft * -qs) + (!isLeft * qs);
        voxelChild->origin[1] = parent->origin[1] + (isBottom * -qs) + (!isBottom * qs);
        voxelChild->origin[2] = parent->origin[2] + (isFront * -qs) + (!isFront * qs);;
        voxel_init_children_as_null(voxelChild);
        return voxelChild;
}

void voxel_add_voxel(Voxel *voxel, unsigned int x, unsigned int y, unsigned int z){
    int isLeft = (x < voxel->origin[0]);
    int isBottom = (y < voxel->origin[1]);
    int isFront = (z < voxel->origin[2]);
    int n = 4 * !isLeft + 2 * !isBottom + !isFront;

    if(voxel->size == 2){
        if(voxel->child[n] == NULL){
            voxel->child[n] = voxel_create_size_1_voxel(x, y, z);
            voxel_do_merge_voxels(voxel);
        }
    }else{
        if(voxel->child[n] == NULL){
            voxel->child[n] = voxel_create_size_gte_2_voxel(voxel,isLeft, isBottom, isFront);
        }                 
        voxel_add_voxel(voxel->child[n], x, y, z);
        voxel_do_merge_voxels(voxel);


    }   
}

int voxel_is_leaf(Voxel *voxel){
    if(voxel->size == 1){
        return 1;
    }
    int nullcount = 0;
    for(int i = 0; i < 8; i++){
        nullcount += voxel->child[i] == NULL;
    }
    return (nullcount == 8);
}

int voxel_count_voxels(Voxel* voxel){
    if(voxel_is_leaf(voxel)){
        return 1;
    }
    int voxelCount = 0;
    for(int i = 0; i < 8; i++){
        if(voxel->child[i] != NULL){
            voxelCount += voxel_count_voxels(voxel->child[i]);
        }
    }

    return voxelCount;
}

void voxel_generate_model(Voxel *voxel, ColorRgbaF color, PosColor32Vertex *vertexArray, unsigned int *elementArray, int *index){
    if(voxel_is_leaf(voxel)){
        vec3 origin, halfExtents;
        PosColor32Vertex verticies[8];
        unsigned int elements[36];
        if(voxel->size == 1){
            origin[0] = voxel->origin[0] + 0.5;
            origin[1] = voxel->origin[1] + 0.5;
            origin[2] = voxel->origin[2] + 0.5;
            halfExtents[0] = 0.5;
            halfExtents[1] = 0.5;
            halfExtents[2] = 0.5;
        }else{
            origin[0] = voxel->origin[0];
            origin[1] = voxel->origin[1];
            origin[2] = voxel->origin[2];
            float ext = voxel->size * 0.5;
            halfExtents[0] = ext;
            halfExtents[1] = ext;
            halfExtents[2] = ext;
        }    
        box_generate_box_pos_color_32_vertex(origin, color, halfExtents, verticies, elements);
        int elementNOffset = (*index) * 8;
        for(int i = 0; i < 36; i++){
            elements[i] += elementNOffset;
        }
        memcpy(&vertexArray[*index * 8], verticies, 8 * sizeof(PosColor32Vertex));
        memcpy(&elementArray[*index * 36], elements, 36 * sizeof(unsigned int));
        *index += 1;
    }else{
        for(int i = 0; i < 8; i++){
            if(voxel->child[i] != NULL){
                voxel_generate_model(voxel->child[i], color, vertexArray, elementArray, index);
            }
        }
    }
}

void voxel_free_children(Voxel *voxel){
    for(int i = 0; i < 8; i++){
        free(voxel->child[i]);
        voxel->child[i] = NULL;
    }
}

void voxel_do_merge_voxels(Voxel *voxel){
    int childCount = 0;

    if(voxel->size == 2){
        for(int i = 0; i < 8; i++){
            childCount += voxel->child[i] != NULL;
        }
    }else{
        for(int i = 0; i < 8; i++){
            if(voxel->child[i] != NULL){
                childCount += voxel_is_leaf(voxel->child[i]);
            }else{
                i = 8;
            }
        }
    }
    if(childCount == 8){
        voxel_free_children(voxel);
    }
}

Aabb voxel_to_aabb_from_child_i(Voxel voxel, int i){
    Aabb aabb;
    int isleft = voxel_index_is_left(i);
    int isBottom = voxel_index_is_bottom(i);
    int isFront = voxel_index_is_front(i);
    if(voxel.size == 2){
        aabb.x = voxel.origin[0] - isleft;
        aabb.y = voxel.origin[1] - isBottom;
        aabb.z = voxel.origin[2] - isFront;
        aabb.w = 1;
        aabb.h = 1;
        aabb.d = 1;
        return aabb;
    }
    int hs = voxel.size / 2;
    aabb.x = voxel.origin[0] + (isleft * -hs);
    aabb.y = voxel.origin[1] + (isBottom * -hs);
    aabb.z = voxel.origin[2] + (isFront * -hs);
    aabb.w = hs;
    aabb.h = hs;
    aabb.d = hs;
    return aabb;
}


void voxel_add_voxels_from_aabb(Voxel *voxel, Aabb aabb, int isNew){
    if(voxel->size == 2){ 
        if(isNew || !voxel_is_leaf(voxel)){
            for(int i = 0; i < 8; i++){
                if(voxel->child[i] == NULL){
                    int isLeft = voxel_index_is_left(i);
                    int isBottom = voxel_index_is_bottom(i);
                    int isFront = voxel_index_is_front(i);
                    float x = (float)(voxel->origin[0]) - isLeft;
                    float y = (float)(voxel->origin[1]) - isBottom;
                    float z = (float)(voxel->origin[2]) - isFront;
                    if(aabb_contains_point_inclusive(aabb, x + 0.5, y + 0.5, z + 0.5)){
                        voxel->child[i] = voxel_create_size_1_voxel(x, y, z);   
                    }  
                }
            }                         
            voxel_do_merge_voxels(voxel); 
        }       
    }else{   
        if(isNew || !voxel_is_leaf(voxel)){
            for(int i = 0; i < 8; i++){              
                vec3 overlaps;
                Aabb voxBox = voxel_to_aabb_from_child_i(*voxel, i); 
                aabb_get_overlaping_area(voxBox, aabb, overlaps);
                int overlapPred = (overlaps[0] >= 0.5) && (overlaps[1] >= 0.5) && (overlaps[2] >= 0.5); //this works
                if(overlapPred){    
                    int newVoxel = 0;                
                    if(voxel->child[i] == NULL){
                        int isLeft = voxel_index_is_left(i);
                        int isBottom = voxel_index_is_bottom(i);
                        int isFront = voxel_index_is_front(i);
                        voxel->child[i] = voxel_create_size_gte_2_voxel(voxel, isLeft, isBottom, isFront);
                        newVoxel = 1;
                    }
                    voxel_add_voxels_from_aabb(voxel->child[i], aabb, newVoxel);    
                          
                }                      
            }
            voxel_do_merge_voxels(voxel); 
        }
    }
}

void voxel_add_voxels_from_sphere(Voxel *voxel, Sphere sphere, int isNew){
    if(voxel->size == 2){

        if(isNew || !voxel_is_leaf(voxel)){
            for(int i = 0; i < 8; i++){
                if(voxel->child[i] == NULL){
                    int isLeft = voxel_index_is_left(i);
                    int isBottom = voxel_index_is_bottom(i);
                    int isFront = voxel_index_is_front(i);
                    float x = (float)(voxel->origin[0]) - isLeft;
                    float y = (float)(voxel->origin[1]) - isBottom;
                    float z = (float)(voxel->origin[2]) - isFront;
                    if(sphere_contains_point_inclusive(sphere, x + 0.5, y + 0.5, z + 0.5)){
                        voxel->child[i] = voxel_create_size_1_voxel(x,y,z);
                    }
                }
            }
            voxel_do_merge_voxels(voxel); 

        }
    }else{
        if(isNew || !voxel_is_leaf(voxel)){
            for(int i = 0; i < 8; i++){
                Aabb voxBox = voxel_to_aabb_from_child_i(*voxel, i); 
                Aabb predBox = aabb_get_box_with_subtracted_corners(voxBox, 0.5f);
                int overlapPred = sphere_intersects_aabb(sphere, predBox);  
                if(overlapPred){    
                    int newVoxel = 0;                
                    if(voxel->child[i] == NULL){
                        int isLeft = voxel_index_is_left(i);
                        int isBottom = voxel_index_is_bottom(i);
                        int isFront = voxel_index_is_front(i);
                        voxel->child[i] = voxel_create_size_gte_2_voxel(voxel, isLeft, isBottom, isFront);
                        newVoxel = 1;
                    }
                    voxel_add_voxels_from_sphere(voxel->child[i], sphere, newVoxel);             
                }                      
            }
            voxel_do_merge_voxels(voxel); 
        }
    }
}

void voxel_regenerate_children(Voxel *voxel){
    if(voxel->size == 2){
        for(int i = 0; i < 8; i++){
            int isLeft = voxel_index_is_left(i);
            int isBottom = voxel_index_is_bottom(i);
            int isFront = voxel_index_is_front(i);
            int index = 4 * !isLeft + 2 * !isBottom + !isFront;
            voxel->child[i] = (Voxel*) malloc(sizeof(Voxel));
            voxel->child[i]->size = 1;
            voxel->child[i]->origin[0] = voxel->origin[0] - isLeft;
            voxel->child[i]->origin[1] = voxel->origin[1] - isBottom;
            voxel->child[i]->origin[2] = voxel->origin[2] - isFront;
            voxel_init_children_as_null(voxel->child[i]);
        }
    }else{
        for(int i = 0; i < 8; i++){
            int isLeft = voxel_index_is_left(i);
            int isBottom = voxel_index_is_bottom(i);
            int isFront = voxel_index_is_front(i);
            int index = 4 * !isLeft + 2 * !isBottom + !isFront;
            int qs = voxel->size / 4;
            voxel->child[i] = (Voxel*) malloc(sizeof(Voxel));
            voxel->child[i]->size = voxel->size / 2;
            voxel->child[i]->origin[0] = voxel->origin[0] + (isLeft * -qs) + (!isLeft * qs);
            voxel->child[i]->origin[1] = voxel->origin[1] + (isBottom * -qs) + (!isBottom * qs);
            voxel->child[i]->origin[2] = voxel->origin[2] + (isFront * -qs) + (!isFront * qs);
            voxel_init_children_as_null(voxel->child[i]);
        }

    }
}

void voxel_delete_voxel(Voxel **voxel, unsigned int x, unsigned int y, unsigned int z){
    Voxel *voxelEntity = (*voxel);
    int isLeft = (x < voxelEntity->origin[0]);
    int isBottom = (y < voxelEntity->origin[1]);
    int isFront = (z < voxelEntity->origin[2]);
    int n = 4 * !isLeft + 2 * !isBottom + !isFront;

    if((voxelEntity->size) == 2){
        if(voxel_is_leaf(*voxel)){     
            voxel_regenerate_children(*voxel);
          
        }
        if(voxelEntity->child[n] != NULL){
            free((*voxel)->child[n]);
            (*voxel)->child[n] = NULL;
        }
        if(voxel_is_leaf(*voxel)){
            *voxel = NULL;
            free(*voxel);
        }
    }else{
        if(voxel_is_leaf(*voxel)){
            voxel_regenerate_children(*voxel);
        }

        if((*voxel)->child[n] != NULL){     
            voxel_delete_voxel(&(*voxel)->child[n], x, y, z); 
        }

        if(voxel_is_leaf(*voxel)){
            *voxel = NULL;
            free(*voxel);
        }
    }
}

void voxel_delete_voxel_from_aabb(Voxel **voxel, Aabb aabb){
    if((*voxel)->size == 2){
        if(voxel_is_leaf(*voxel)){
            voxel_regenerate_children(*voxel);
        }
        for(int i = 0; i < 8; i++){
            if((*voxel)->child[i] != NULL){
                float x, y, z;
                x = (*voxel)->child[i]->origin[0] + 0.5;
                y = (*voxel)->child[i]->origin[1] + 0.5;
                z = (*voxel)->child[i]->origin[2] + 0.5;
                if(aabb_contains_point(aabb, x, y, z)){
                    free((*voxel)->child[i]);
                    (*voxel)->child[i] = NULL;
                }
            }
        }
        if(voxel_is_leaf(*voxel)){
            free(*voxel);
            *voxel = NULL;
        }
    }else{
        if(voxel_is_leaf(*voxel)){
            voxel_regenerate_children(*voxel);
        }
        for(int i = 0; i < 8; i++){
            if((*voxel)->child[i] != NULL){
                Aabb voxBox = voxel_to_aabb(*(*voxel)->child[i]);
                vec3 overlap;
                aabb_get_overlaping_area(voxBox, aabb, overlap);
                if((overlap[0] >= 0.5) && (overlap[1] >= 0.5) && (overlap[2] >= 0.5)){
                    voxel_delete_voxel_from_aabb(&(*voxel)->child[i], aabb);
                }
            }
        }
        if(voxel_is_leaf(*voxel)){
            free(*voxel);
            *voxel = NULL;
        }
    }
}

void voxel_delete_voxel_from_sphere(Voxel **voxel, Sphere sphere){
    if((*voxel)->size == 2){
        if(voxel_is_leaf(*voxel)){
            voxel_regenerate_children(*voxel);
        }
        for(int i = 0; i < 8; i++){
            if((*voxel)->child[i] != NULL){
                float x, y, z;
                x = (*voxel)->child[i]->origin[0] + 0.5;
                y = (*voxel)->child[i]->origin[1] + 0.5;
                z = (*voxel)->child[i]->origin[2] + 0.5;
                if(sphere_contains_point(sphere, x, y, z)){
                    free((*voxel)->child[i]);
                    (*voxel)->child[i] = NULL;
                }
            }
        }
        if(voxel_is_leaf(*voxel)){
            free(*voxel);
            *voxel = NULL;
        }
    }else{
        if(voxel_is_leaf(*voxel)){
            voxel_regenerate_children(*voxel);
        }
        for(int i = 0; i < 8; i++){
            if((*voxel)->child[i] != NULL){
                Aabb voxBox = voxel_to_aabb_from_child_i(**voxel, i); 
                Aabb predBox = aabb_get_box_with_subtracted_corners(voxBox, 0.5f);
                int overlapPred = sphere_intersects_aabb(sphere, predBox);  
                if(overlapPred){
                    voxel_delete_voxel_from_sphere(&(*voxel)->child[i], sphere);
                }
            }
        }
        if(voxel_is_leaf(*voxel)){
            free(*voxel);
            *voxel = NULL;
        }
    }
}