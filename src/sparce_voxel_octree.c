#include "../include/sparce_voxel_octree.h"


int sparce_voxel_index_is_left(int index){
    return (index == 0) || (index == 2) || (index == 4) || (index == 6);
}

int sparce_voxel_index_is_bottom(int index){
    return (index == 0) || (index == 1) || (index == 4) || (index == 5);
}

int sparce_voxel_index_is_front(int index){
    return (index == 0) || (index == 1) || (index == 2) || (index == 3);
}

SparceVoxel16x16x16Compressed* sparce_voxel_16x16x16_compress(SparceVoxel16x16x16 *sparceVoxel){
  
    SparceVoxel16x16x16Compressed *encodedVoxel = (SparceVoxel16x16x16Compressed*) malloc(sizeof(SparceVoxel16x16x16Compressed));
    
    memcpy(encodedVoxel->min, sparceVoxel->min, sizeof(int) * 3);
    encodedVoxel->type = SPARCE_VOXEL_16X16X16_COMPRESSED;
    encodedVoxel->rleHead = NULL;
    unsigned int *colorData = &sparceVoxel->colorData[0][0][0][0];

    //Set First Color;
    encodedVoxel->rleHead = (ColorEncodingNode*) malloc(sizeof(ColorEncodingNode));
    memcpy(
        encodedVoxel->rleHead->encoding.color,
        &colorData[0],
        sizeof(ColorRGBA)
    );
    
    encodedVoxel->rleHead->encoding.colorCount = 1;
    encodedVoxel->rleHead->nextColor = NULL;

    for(int i = 1; i < 16 * 16 * 16; i++){
        unsigned int colorInt = colorData[i];
        if(colorData[i] == colorInt){
            encodedVoxel->rleHead->encoding.colorCount == 1;
        }else{
            ColorEncodingNode *newColor = (ColorEncodingNode*) malloc(sizeof(ColorEncodingNode));
            newColor->nextColor = NULL;
            newColor->encoding.colorCount = 1;
            memcpy(&newColor->encoding.color, &colorInt, sizeof(unsigned int));
            newColor->nextColor = encodedVoxel->rleHead;
            encodedVoxel->rleHead = newColor;
        }
    }
    return encodedVoxel;
}

SparceVoxel16x16x16* sparce_voxel_16x16x16_decompress(SparceVoxel16x16x16Compressed *sparceVoxel){

}

Aabb voxel_large_get_child_aabb(SparceVoxelLarge *voxel, int childId){
    int isLeft = sparce_voxel_index_is_left(childId); 
    int isBottom = sparce_voxel_index_is_bottom(childId); 
    int isFront = sparce_voxel_index_is_front(childId); 
    int hs = voxel->size / 2;
    Aabb aabb = {
        voxel->origin[0], voxel->origin[1], 
        voxel->origin[2] ,hs, hs, hs
    };
    aabb.x -= hs * isLeft;
    aabb.y -= hs * isBottom;
    aabb.z -= hs * isFront;

    return aabb;
}

void sparce_voxel_large_edit_size_32(SparceVoxelLarge **voxel,SparceVoxelOctreeEditor *editor){
    printf("edit voxel of size 32 \n");

    for(int i = 0; i < 8; i++){
        Aabb voxBox = voxel_large_get_child_aabb(*voxel, i);
        Aabb predBox = aabb_from_expand(voxBox, -0.5);

        if(shape_intersects_aabb(editor->shapeType, editor->shapeData, predBox)){
            printf("create new voxel grid \n");
            
            if((*voxel)->child[i] == NULL){
                (*voxel)->child[i] = (SparceVoxel16x16x16*) malloc(sizeof(SparceVoxel16x16x16));
                SparceVoxel16x16x16* child = (*voxel)->child[i];
                child->type = SPARCE_VOXEL_16X16X16;
                for(int x = 0; x < 16; x++){
                    for(int y = 0; y < 16; y++){
                        for(int z = 0; z < 16; z++){
                            int* p = &child->colorData[x][y][z];
                            *p = 0;   
                        }
                    }
                }
                child->min[0] = (int)voxBox.x;
                child->min[1] = (int)voxBox.y;
                child->min[2] = (int)voxBox.z;
            }
            sparce_voxel_edit_16x16x16(&(*voxel)->child[i], editor);            
        }
    }
}

void voxel_large_null_children(SparceVoxelLarge *voxelLarge){
    for(int i = 0; i < 8; i++){
        voxelLarge->child[i] = NULL;
    }
}

void sparce_voxel_large_edit_default(SparceVoxelLarge **voxel, SparceVoxelOctreeEditor *editor){
    printf("edit mega voxel\n");
    for(int i = 0; i < 8; i++){
        Aabb voxBox = voxel_large_get_child_aabb(*voxel, i);
        Aabb predBox = aabb_from_expand(voxBox, -0.5);
        if(shape_intersects_aabb(editor->shapeType, editor->shapeData, predBox)){
            if((*voxel)->child[i] == NULL){
                vec3 childOrigin;
                aabb_get_origin(voxBox,childOrigin);
                (*voxel)->child[i] = (SparceVoxelLarge*) malloc(sizeof(SparceVoxelLarge));
                SparceVoxelLarge* child = (*voxel)->child[i];
                child->type = SPARCE_VOXEL_LARGE;
                child->size = (*voxel)->size / 2;
                child->origin[0] = (int)childOrigin[0];
                child->origin[1] = (int)childOrigin[1];
                child->origin[2] = (int)childOrigin[2];
                voxel_large_null_children(child);   
            }
            sparce_voxel_edit_large(&(*voxel)->child[i], editor);
        }
    }
}

void sparce_voxel_edit_large(SparceVoxelLarge **voxel, SparceVoxelOctreeEditor *editor){
    printf("edit large voxel \n");    
 
    switch ((*voxel)->size){
        case 32:
            sparce_voxel_large_edit_size_32(voxel, editor);
            break;
        default:
            sparce_voxel_large_edit_default(voxel, editor);
            break;
        
    }
}

void sparce_voxel_edit_16x16x16(SparceVoxel16x16x16 **voxel, SparceVoxelOctreeEditor *editor){
    printf("edit 16x16x16\n");
    
    //Decode this Voxel;
    SparceVoxel16x16x16 *editingVoxel = sparce_voxel_16x16x16_decompress(*voxel);
    if((*voxel)->type == SPARCE_VOXEL_16X16X16_COMPRESSED){

        //Free the Run Lenght Encoded Voxels
        free(*voxel);
        *voxel = NULL;
    }

    
    //Add Voxels Based on Shape
    Aabb shapeBox = shape_to_aabb(editor->shapeType, editor->shapeData);

    int xOffset = (*voxel)->min[0];
    int yOffset = (*voxel)->min[1];
    int zOffset = (*voxel)->min[2];

    int minX = (int)floorf(shapeBox.x) - xOffset;
    int maxX = (int)ceilf(shapeBox.x + shapeBox.width) - xOffset;

    int minY = (int)floorf(shapeBox.y) - yOffset;
    int maxY = (int)ceilf(shapeBox.y + shapeBox.height) - yOffset;
    
    int minZ = (int)floorf(shapeBox.z) -zOffset;
    int maxZ = (int)ceilf(shapeBox.z + shapeBox.depth) - zOffset;
    for(int x = (int)fmaxf(minX, 0); x < fminf(maxX, 16); x++){
        for(int y = fmaxf(minY, 0); y < fminf(maxY, 16); y++){
            for(int z = fmaxf(minZ, 0); z < fminf(maxZ, 16); z++){
                if(shape_contains_point(editor->shapeType, editor->shapeData, 
                    x + 0.5 + xOffset, y + 0.5 + yOffset, z + 0.5 + zOffset)){     
                    
                    colorRgba_copy((*voxel)->colorData[x][y][z], editor->brushColor);    
                }
            }
        }
    }
    //Encode this Voxel

    /*
    //Free the Voxel Array
    free(editingVoxel);
    editingVoxel = NULL;
    */
}

void sparce_voxel_edit(SparceVoxel **voxel, SparceVoxelOctreeEditor *editor){
    switch ((*voxel)->type)
    {
    case SPARCE_VOXEL_LARGE:
        sparce_voxel_edit_large(voxel, editor);
        break;
    case SPARCE_VOXEL_16X16X16:
        sparce_voxel_edit_16x16x16(voxel, editor);
        break;
    }
}

Aabb voxel_large_get_aabb(SparceVoxelLarge *voxelLarge){
    int hs = voxelLarge->size / 2;
    Aabb aabb = {
        voxelLarge->origin[0] - hs, 
        voxelLarge->origin[1] - hs, 
        voxelLarge->origin[2] - hs,
        voxelLarge->size,
        voxelLarge->size,
        voxelLarge->size
    };
    return aabb;
}

void sparce_voxel_octree_expand_empty(SparceVoxelOctree *octree, SparceVoxelOctreeEditor *editor){
    Aabb voxBox = voxel_large_get_aabb((SparceVoxelLarge*)octree->head);
    Aabb shapeAabb = shape_to_aabb(editor->shapeType, editor->shapeData);
    vec3 corners[8];
    aabb_get_corners(shapeAabb, corners);

    for(int i = 0; i < 8; i++){
        int n = 0;
        while((n < 100) && !aabb_contains_point(voxBox, corners[i][0], corners[i][1], corners[i][2])){
            ((SparceVoxelLarge*)octree->head)->size *= 2;
            voxBox = voxel_large_get_aabb((SparceVoxelLarge*)octree->head);
            n++;
        }
    }
}
void sparce_voxel_octree_expand_head(SparceVoxelOctree *octree, SparceVoxelOctreeEditor *editor){
    Aabb voxBox = voxel_large_get_aabb((SparceVoxelLarge*)octree->head);
    Aabb shapeAabb = shape_to_aabb(editor->shapeType, editor->shapeData);
    vec3 corners[8];
    aabb_get_corners(shapeAabb, corners);

    for(int i = 0; i < 8; i++){
        vec3 vectorToPoint, trajectory;
        vec3_sub(vectorToPoint, corners[i], voxBox.min);
        trajectory[0] =  (vectorToPoint[0] >= 0) * 1 + (vectorToPoint[0] < 0) * -1;
        trajectory[1] =  (vectorToPoint[1] >= 0) * 1 + (vectorToPoint[1] < 0) * -1;
        trajectory[2] =  (vectorToPoint[2] >= 0) * 1 + (vectorToPoint[2] < 0) * -1;

        int childNumber = 7;
        childNumber -= (trajectory[0] == 1);
        childNumber -= 2 * (trajectory[1] == 1);
        childNumber -= 4 * (trajectory[2] == 1);

        while (!aabb_contains_point(voxBox, corners[i][0], corners[i][1], corners[i][2])){
            SparceVoxelLarge* newHead = (SparceVoxelLarge*) malloc(sizeof(SparceVoxelLarge));
            SparceVoxelLarge* oldHead = (SparceVoxelLarge*)octree->head;
            newHead->size = oldHead->size * 2;
            int hs = oldHead->size / 2;
            newHead->origin[0] = oldHead->origin[0] + hs * trajectory[0];
            newHead->origin[1] = oldHead->origin[1] + hs * trajectory[1];
            newHead->origin[2] = oldHead->origin[2] + hs * trajectory[2];
            newHead->type = SPARCE_VOXEL_LARGE;
            voxel_large_null_children(newHead);
            newHead->child[childNumber] = oldHead;
            octree->head = newHead;
            voxBox = voxel_large_get_aabb((SparceVoxelLarge*)octree->head);
        }
    }
}

void sparce_voxel_octree_add(SparceVoxelOctree *octree, SparceVoxelOctreeEditor *editor){
    int wasEmpty = 0;

    //If Generate a new tree with a size of 32

    if(octree->head == NULL){
        octree->head = (SparceVoxelLarge*) malloc(sizeof(SparceVoxelLarge));
        SparceVoxelLarge* head = octree->head;
        head->type = SPARCE_VOXEL_LARGE;
        head->size = 32;
        head->origin[0] = 0;
        head->origin[1] = 0;
        head->origin[2] = 0;
        for(int i = 0; i < 8; i++){
            head->child[i] = NULL;
        }
        wasEmpty = 1;

    }
    
    switch (wasEmpty)
    {
    case 1:  //If was empty expand the tree until the shape can fit
        sparce_voxel_octree_expand_empty(octree, editor);
        break;
    case 0: //If the tree is not empty add larger nodes to the tree unitl it can fit 
        sparce_voxel_octree_expand_head(octree, editor);
        break;
    }
    sparce_voxel_edit(&octree->head, editor);
    
}



void sparce_voxel_octree_edit(SparceVoxelOctree *octree, SparceVoxelOctreeEditor *editor){
    printf("sparce voxel octree edit \n");
    switch (editor->action)
    {
    case VOXEL_ADD:
        sparce_voxel_octree_add(octree, editor);
        break;
    case VOXEL_DELETE:
        //sparce_voxel_octree_delete(octree, editor);
        break;
    }
}
