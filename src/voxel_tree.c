#include "../include/voxel_tree.h"


void voxel_tree_double_empty(VoxelTree *voxelTree){
    voxelTree->head->size *= 2;
    for(int i = 0; i < 3; i++){
        voxelTree->head->origin[i] = voxelTree->head->origin[i] * 2;
    }
}

void voxel_tree_expand_empty(VoxelTree* voxelTree, unsigned int x, unsigned int y, unsigned int z){
    int i = 0;
    while (!aabb_contains_point(voxel_to_aabb(*voxelTree->head), x, y, z) && i < 1000){
        voxel_tree_double_empty(voxelTree);
        i++;
    }
}

void voxel_tree_init_head(VoxelTree *voxelTree){
    voxelTree->head = (Voxel*) malloc(sizeof(Voxel));
    voxelTree->head->size = 2;
    for(int i = 0; i < 3; i++){
        voxelTree->head->origin[i] = 1;
    }
    voxel_init_children_as_null(voxelTree->head);
}

void voxel_tree_expand_head(VoxelTree *voxelTree, unsigned int x, unsigned int y, unsigned int z){
    int i = 0;
    while (!aabb_contains_point(voxel_to_aabb(*voxelTree->head), x, y, z) && i < 1000){
        Voxel* newHead = (Voxel*) malloc(sizeof(Voxel));
        newHead->size = voxelTree->head->size * 2;
        for(int i = 0; i < 3; i++){
            newHead->origin[i] = voxelTree->head->origin[i] * 2;  
        }
        voxel_init_children_as_null(newHead);
        newHead->child[0] = voxelTree->head;
        voxelTree->head = newHead;
        i++;
    }
}
void shape_get_maxPoint(int shapeId, float *geometry, vec3 point){
    switch (shapeId)
    {
    case SHAPE_ID_CUBE:
        {
            Aabb aabb;
            memcpy(&aabb, geometry, sizeof(Aabb));
            aabb_get_max_corner(aabb, point);
        }
        break;
    case SHAPE_ID_SPHERE:
        {
            Sphere sphere;
            memcpy(&sphere, geometry, sizeof(Sphere));
            sphere_get_box_max_point(sphere, point);
        }
        break;
    }
}

void voxel_tree_add_voxel_from_shape(VoxelTree *voxelTree, int shapeId, float* geometry){
    Aabb voxBox;
    vec3 maxPoint;
    shape_get_maxPoint(shapeId, geometry, maxPoint);
    if(voxelTree->head == NULL){
        voxel_tree_init_head(voxelTree);
        voxel_tree_expand_empty(voxelTree, maxPoint[0], maxPoint[1], maxPoint[2]);
    }else{
        voxel_tree_expand_head(voxelTree, maxPoint[0], maxPoint[1], maxPoint[2]);
    }
    voxel_add_voxels_from_shape(voxelTree->head, shapeId, geometry, 1);
}

void voxel_tree_delete_voxel_from_shape(VoxelTree *voxelTree,int shapeId, float *geometry){
    if(voxelTree->head != NULL){
        voxel_delete_voxels_from_shape(&voxelTree->head, shapeId, geometry);
    }
}

void voxel_tree_expand_empty_for_aabb(VoxelTree *voxelTree, Aabb aabb){
    int i = 0;
    vec3 maxCorner;
    aabb_get_max_corner(aabb, maxCorner);
    Aabb voxBox = voxel_to_aabb(*voxelTree->head);

    while (!aabb_contains_point_inclusive(voxBox, maxCorner[0], maxCorner[1], maxCorner[2]) && i < 1000)
    {
        voxel_tree_double_empty(voxelTree);
        voxBox = voxel_to_aabb(*voxelTree->head);
    }
}


void voxel_add_voxels_from_shape(Voxel *voxel, int shapeId, float *geometry, int isNew){
     if(voxel->size == 2){ 
        if(isNew || !voxel_is_leaf(voxel)){
            for(int i = 0; i < 8; i++){
                if(voxel->child[i] == NULL){
                    int isLeft = voxel_index_is_left(i);
                    int isBottom = voxel_index_is_bottom(i);
                    int isFront = voxel_index_is_front(i);
                    float x = (float)(voxel->origin[0]) - isLeft + 0.5;
                    float y = (float)(voxel->origin[1]) - isBottom + 0.5;
                    float z = (float)(voxel->origin[2]) - isFront + 0.5;
                    if(shape_contains_point(shapeId, geometry, x, y, z)){
                        int location[3] = {x,y,z};
                        voxel->child[i] = voxel_create(voxel, location);
                        

                    }
                }
            }                         
            voxel_do_merge_voxels(voxel); 
        }       
    }else{   
        if(isNew || !voxel_is_leaf(voxel)){
            for(int i = 0; i < 8; i++){              
                Aabb voxBox = voxel_to_aabb_from_child_i(*voxel, i); 
                int overlapPred = shape_overlaps(shapeId, geometry, voxBox);
                
                if(overlapPred){    
                    int newVoxel = 0;                
                    if(voxel->child[i] == NULL){
                        int isLeft = voxel_index_is_left(i);
                        int isBottom = voxel_index_is_bottom(i);
                        int isFront = voxel_index_is_front(i);
                        int placement[3] = {isLeft, isBottom, isFront};
                        voxel->child[i] = voxel_create(voxel, placement );
                        newVoxel = 1;
                    }
                    voxel_add_voxels_from_shape(voxel->child[i], shapeId, geometry, newVoxel);
                }                      
            }
            voxel_do_merge_voxels(voxel); 
        }
    }
}


void voxel_tree_expand_head_for_aabb(VoxelTree *voxelTree, Aabb aabb){
    vec3 maxCorner;
    aabb_get_max_corner(aabb, maxCorner);
    Aabb voxBox = voxel_to_aabb(*voxelTree->head);
    int i = 0;
    voxel_tree_expand_head(voxelTree, (unsigned int)ceilf(maxCorner[0]), (unsigned int)ceilf(maxCorner[1]), (unsigned int)ceilf(maxCorner[2]));
}


void voxel_tree_free_all_children(VoxelTree *voxelTree){
    if(voxelTree->head != NULL){
        voxel_free_all_children(&voxelTree->head);
    }
}


int shape_contains_point(int shapeId, float *geometry, float x, float y, float z){
    switch (shapeId)
    {
    case SHAPE_ID_CUBE: 
        {
        Aabb aabb;
        memcpy(&aabb, geometry, sizeof(Aabb));
        return aabb_contains_point_inclusive(aabb, x, y, z);
        }
    case SHAPE_ID_SPHERE:
        {
        Sphere sphere;
        memcpy(&sphere, geometry, sizeof(Sphere));
        return sphere_contains_point_inclusive(sphere, x, y, z);
        }
    }
}

void voxel_delete_voxels_from_shape(Voxel **voxel, int shapeId, float* geometry){
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
                
                if(shape_contains_point(shapeId, geometry, x, y, z)){
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
                if(shape_overlaps(shapeId, geometry, voxBox)){
                    voxel_delete_voxels_from_shape(&(*voxel)->child[i], shapeId, geometry);
                }
            }
        }
        if(voxel_is_leaf(*voxel)){
            free(*voxel);
            *voxel = NULL;
        }
    }
}

int shape_overlaps(int shapeId, float *geometry, Aabb voxBox){
    switch (shapeId)
    {
    case SHAPE_ID_CUBE: 
        {
        vec3 overlaps;
        Aabb aabb;
        memcpy(&aabb, geometry, sizeof(Aabb));
        aabb_get_overlaping_area(voxBox, aabb, overlaps);
        return (overlaps[0] >= 0.5) && (overlaps[1] >= 0.5) && (overlaps[2] >= 0.5);
        }
    
    case SHAPE_ID_SPHERE:
        {
        Aabb predBox = aabb_get_box_with_subtracted_corners(voxBox, 0.5f);
        Sphere sphere;
        memcpy(&sphere, geometry, sizeof(Sphere));
        return sphere_intersects_aabb(sphere, predBox);  
        }
    }
};






void voxel_copy_from_shape(Voxel *giver, VoxelTree *reciever, int shapeId, float *geometry){
    if(giver->size == 1){
        float x = (float)(giver->origin[0]) + 0.5;
        float y = (float)(giver->origin[1]) + 0.5;
        float z = (float)(giver->origin[2]) + 0.5;
        
        if(shape_contains_point(shapeId,geometry, x, y, z)){
            float box[6] = {floorf(x), floorf(y), floorf(z), 1, 1, 1};
            voxel_tree_add_voxel_from_shape(reciever, SHAPE_ID_CUBE, box);
        }
    }else{
        Aabb voxBox = voxel_to_aabb(*giver);
        if(shape_overlaps(shapeId, geometry, voxBox)){
            int isLeaf = voxel_is_leaf(giver);
            if(isLeaf){
                voxel_regenerate_children(giver);
            }
            for(int i = 0; i < 8; i++){
                if(giver->child[i] != NULL){
                    voxel_copy_from_shape(giver->child[i], reciever, shapeId, geometry);
                }
            }  
            if(isLeaf){
                voxel_do_merge_voxels(giver);
            }
        }
    }
}


void voxel_tree_copy_from_shape(VoxelTree *giver, VoxelTree *reciever, int shapeId, float *geometry){
        voxel_copy_from_shape(giver->head, reciever, shapeId, geometry);

}
void voxel_tree_cut_from_shape(VoxelTree *giver, VoxelTree *reciever, int shapeId, float *geometry){
    voxel_copy_from_shape(giver->head, reciever, shapeId, geometry);
    voxel_tree_delete_voxel_from_shape(giver, shapeId, geometry);
}


void voxel_tree_modify_from_voxel_tree(VoxelTree *giver, VoxelTree *reciever, int modification){
    voxel_modify_from_tree(giver->head, giver->offset, reciever, modification);
}


void voxel_do_selected_modifictation(Voxel *secondaryTree, VoxelTree *mainTree, int modification, float *geometry){
    switch(modification){
        case VOXEL_MOD_PASTE:     
            voxel_tree_add_voxel_from_shape(mainTree, SHAPE_ID_CUBE, geometry);
            break;
        case VOXEL_MOD_COPY:
            voxel_tree_copy_from_shape(mainTree, secondaryTree, SHAPE_ID_CUBE, geometry);
            break;
        case VOXEL_MOD_CUT:
            voxel_tree_cut_from_shape(mainTree, secondaryTree, SHAPE_ID_CUBE, geometry);
            break;
    }
}

void voxel_modify_from_tree(Voxel *secondaryTree, vec3 offset, VoxelTree *mainTree, int modification){
    Aabb voxBox = voxel_to_aabb(*secondaryTree);
    vec3_add(voxBox.min, voxBox.min, offset);
    float geometry[6];
    memcpy(geometry, &voxBox, sizeof(Aabb));
    if(secondaryTree->size == 1){
        voxel_do_selected_modifictation(secondaryTree, mainTree, modification, geometry);
    }else{
        if(voxel_is_leaf(secondaryTree)){
            voxel_do_selected_modifictation(secondaryTree, mainTree, modification, geometry);
        }else{
            for(int i = 0; i < 8; i++){
                if(secondaryTree->child[i] != NULL){
                    voxel_modify_from_tree(secondaryTree->child[i], offset, mainTree, modification);
                }
            }
        }
    }
}


int voxel_tree_count_voxels(VoxelTree *voxelTree){
    if(voxelTree->head){
        return voxel_count_voxels(voxelTree->head);
    }
    return 0;
}
/*
void voxel_tree_generate_model(VoxelTree voxelTree, float *vertexArray, unsigned int *elementArray){
    if(voxelTree.head != NULL){
        int index = 0;
        ColorRgbaF color = {0.0,1.0,0.0,1.0};
        voxel_generate_model(voxelTree.head, color, vertexArray, elementArray, &index);
    }
}
*/

int voxel_tree_is_point_in_model(VoxelTree *voxelTree, float x, float y, float z){
    if(voxelTree->head != NULL){
        return voxel_is_point_in_voxel(voxelTree->head, x, y, z);
    }
    return 0;
}

void voxel_tree_voxel_hit_correction(int side, int action , float *x, float *y, float *z){
    switch (side)
    {
    case AABB_SIDE_LEFT:
        *x = roundf(*x) + (- 1 * (action == VOXEL_EDIT_MODE_ID_ADD));
        break;
    case AABB_SIDE_RIGHT:
        *x = roundf(*x) + (-1 * (action == VOXEL_EDIT_MODE_ID_DELETE));
        break;
    case AABB_SIDE_BOTTOM:
        *y = roundf(*y) + (- 1 * (action == VOXEL_EDIT_MODE_ID_ADD));
        break;
    case AABB_SIDE_TOP:
        *y = roundf(*y) + (-1 * (action == VOXEL_EDIT_MODE_ID_DELETE));
        break;
    case AABB_SIDE_FRONT:
        *z = roundf(*z) + (- 1 * (action == VOXEL_EDIT_MODE_ID_ADD));
        break;
    case AABB_SIDE_BACK:
        *z = roundf(*z) + (-1 *(action == VOXEL_EDIT_MODE_ID_DELETE));
        break;
    }
}

void voxel_tree_pick_cell(int hitSide,int hitEntity,int editMode, vec3 hitpoint, float *cellX, float *cellY, float *cellZ){
    *cellX = -1.0;
    *cellY = -1.0;
    *cellZ = -1.0;
    switch (hitEntity)
    {
    case RAY_HIT_ENTITY_Y0:
        *cellX = hitpoint[0];
        *cellY = 0.0f;
        *cellZ = hitpoint[2];
        break;
    case RAY_HIT_ENTITY_VOXEL_MODEL:

        *cellX = hitpoint[0];
        *cellY = hitpoint[1];
        *cellZ = hitpoint[2];

        voxel_tree_voxel_hit_correction(hitSide, editMode, cellX, cellY, cellZ);

        break;
    }
}

int voxel_tree_add_voxels_from_voxel_editor(VoxelTree *voxelTree, VoxelEditor *voxelEditor, vec3 hitpoint ,int hitEntity,int hitSide,
    float cellX, float cellY, float cellZ){

    switch (voxelEditor->shape)
    {
    case VOXEL_EDITOR_SHAPE_ID_POINT:
        if(cellX >= 0 && cellY >= 0 && cellZ >= 0){
            float box[6] = {floorf(cellX), floorf(cellY), floorf(cellZ), 1, 1, 1};
            voxel_tree_add_voxel_from_shape(voxelTree, SHAPE_ID_CUBE, box );
            return 1;
        }
        break;
    case VOXEL_EDITOR_SHAPE_ID_CUBE:
        {
            vec3 maxCorner;
            voxelEditor->aabb.x = cellX;
            voxelEditor->aabb.y = cellY;
            voxelEditor->aabb.z = cellZ;
            aabb_get_max_corner(voxelEditor->aabb, maxCorner);
            if(maxCorner[0] >= 0.5 && maxCorner[1] >= 0.5 && maxCorner[2] >= 0.5){
                float box[6];
                memcpy(&box, &voxelEditor->aabb, sizeof(Aabb));
                voxel_tree_add_voxel_from_shape(voxelTree, SHAPE_ID_CUBE, box );
                return 1;
            }
        }   
        break;
    case VOXEL_EDITOR_SHAPE_ID_SPHERE:
        {
            voxelEditor->sphere.x = cellX + 0.5;
            voxelEditor->sphere.y = cellY + 0.5;
            voxelEditor->sphere.z = cellZ + 0.5;
            
            Aabb testBox = {0.5, 0.5, 0.5, 
                voxelEditor->sphere.x + voxelEditor->sphere.r + 0.5,
                voxelEditor->sphere.y + voxelEditor->sphere.r + 0.5, 
                voxelEditor->sphere.z + voxelEditor->sphere.r + 0.5
            };
            if(sphere_intersects_aabb(voxelEditor->sphere, testBox)){
                float sph[6];
                memcpy(sph, &voxelEditor->sphere, sizeof(Sphere));
                voxel_tree_add_voxel_from_shape(voxelTree, SHAPE_ID_SPHERE, sph );
                return 1;
            }
        }
        break;
    }
    return 0;
}

int voxel_tree_delete_voxel_from_voxel_editor(VoxelTree *voxelTree, VoxelEditor *voxelEditor, vec3 hitpoint ,int hitEntity,int hitSide,
    float cellX, float cellY, float cellZ){

    switch (voxelEditor->shape)
    {
    case VOXEL_EDITOR_SHAPE_ID_POINT:
        if(cellX >= 0 && cellY >= 0 && cellZ >= 0){
            float geometry[6] = {floorf(cellX), floorf(cellY), floorf(cellZ), 1, 1 ,1};
            voxel_tree_delete_voxel_from_shape(voxelTree, SHAPE_ID_CUBE, geometry);
            return 1;
        }
        break;
    case VOXEL_EDITOR_SHAPE_ID_CUBE:
        {
            vec3 maxCorner;
            voxelEditor->aabb.x = cellX;
            voxelEditor->aabb.y = cellY;
            voxelEditor->aabb.z = cellZ;
            aabb_get_max_corner(voxelEditor->aabb, maxCorner);
            if(maxCorner[0] >= 0.5 && maxCorner[1] >= 0.5 && maxCorner[2] >= 0.5){
                float geometry[6];
                memcpy(geometry, &voxelEditor->aabb, sizeof(Aabb));
                voxel_tree_delete_voxel_from_shape(voxelTree, SHAPE_ID_CUBE, geometry);
                return 1;
            }
        }   
        break;
    case VOXEL_EDITOR_SHAPE_ID_SPHERE:
        {
            voxelEditor->sphere.x = cellX + 0.5;
            voxelEditor->sphere.y = cellY + 0.5;
            voxelEditor->sphere.z = cellZ + 0.5;
            
            Aabb testBox = {0.5, 0.5, 0.5, 
                voxelEditor->sphere.x + voxelEditor->sphere.r + 0.5,
                voxelEditor->sphere.y + voxelEditor->sphere.r + 0.5, 
                voxelEditor->sphere.z + voxelEditor->sphere.r + 0.5
            };
            if(sphere_intersects_aabb(voxelEditor->sphere, testBox)){
                float geometry[6];
                memcpy(geometry, &voxelEditor->sphere, sizeof(Sphere));
                voxel_tree_delete_voxel_from_shape(voxelTree, SHAPE_ID_SPHERE, geometry);

                return 1;
            }
        }
        break;
    }
    return 0;
}

int voxel_tree_add_or_delete_from_voxel_editor(VoxelTree *voxelTree, VoxelEditor *voxelEditor, int buttonState, vec3 hitpoint, int hitentity, int hitside){
    int addSuccess, deleteSuccess;
    float cellX = -1;
    float cellY = -1;
    float cellZ = -1;
    voxel_tree_pick_cell(hitside, hitentity, voxelEditor->editMode, hitpoint, &cellX, &cellY, &cellZ);
    switch (voxelEditor->editMode)
    {
        case VOXEL_EDIT_MODE_ID_ADD:
            addSuccess = voxel_tree_add_voxels_from_voxel_editor(voxelTree, voxelEditor, hitpoint, hitentity, hitside,
            cellX, cellY, cellZ);
            break;
        
        case VOXEL_EDIT_MODE_ID_DELETE:
            deleteSuccess = voxel_tree_delete_voxel_from_voxel_editor(voxelTree, voxelEditor, hitpoint, hitentity, hitside,
            cellX, cellY, cellZ);
            break;
    }
    return addSuccess || deleteSuccess;
}

int voxel_tree_edit_voxels_from_voxel_editor(VoxelTree *voxelTree, VoxelEditor *voxelEditor, int buttonState, vec3 hitpoint, int hitentity, int hitside, float deltaTime){ 
    int changed = 0;
    switch (voxelEditor->dynamics)
    {
    case VOXEL_EDIT_DYNAMIC_PLOP:
        if(buttonState == BUTTON_STATE_PRESSED){
            changed = voxel_tree_add_or_delete_from_voxel_editor(voxelTree, voxelEditor, buttonState, hitpoint, hitentity, hitside);
        }
        break;
    case VOXEL_EDIT_DYNAMIC_FLOW:       
        {     
            float t = 1 / voxelEditor->flowRate;
            if(buttonState == BUTTON_STATE_PRESSED || buttonState == BUTTON_STATE_DOWN){

                voxelEditor->flowTick += deltaTime;
                if(voxelEditor->flowTick >= t){
                    changed = voxel_tree_add_or_delete_from_voxel_editor(voxelTree, voxelEditor, buttonState, hitpoint, hitentity, hitside);
                    voxelEditor->flowTick -= t;
                }  
            }else{
                voxelEditor->flowTick = 0;
            }   
        }          
        break;
    }
    return changed;  
}