#include "../include/voxel_object.h"

/*
void voxel_object_init(VoxelObject *voxelObject){
    voxelObject->sparceVoxelOctree.head = NULL;
}


void voxel_object_edit(VoxelObject *voxelObject, VoxelObjectEditor voxelObjectEditor){
    printf("editing voxel object \n");

    //sparce_voxel_octree_edit(&voxelObject->sparceVoxelOctree, &voxelObjectEditor.sparceVoxelOctreeEditor);
}
*/
/*

void voxel_object_add_color_tree(VoxelObject *voxelObject, char color[4]){
    voxelObject->voxelColorTreeCount += 1;
    voxelObject->voxelColorTree = (VoxelColorTree*) realloc(
        voxelObject->voxelColorTree, 
        sizeof(VoxelColorTree) * voxelObject->voxelColorTreeCount  
    );
    memcpy(
        &voxelObject->voxelColorTree[voxelObject->voxelColorTreeCount -1].color,
        color,
        sizeof(char) * 4
    );
}

int voxel_object_find_color_index(VoxelObject *voxelObject, unsigned char color[4]){
    int colorInt = 0;
    memcpy(&colorInt, color, sizeof(unsigned char) * 4);
    for(int i = 0; i < voxelObject->voxelColorTreeCount; i++){
        int iColorInt = 0;
        memcpy(&iColorInt,voxelObject->voxelColorTree[i].color, sizeof(char) * 4);
        if(colorInt == iColorInt){
            return i;
        }
    }
    return -1;
}

void voxel_object_edit(VoxelObject *voxelObject, VoxelObjectEditor editor){
    printf("voxel object edit\n");
    
    int treeI = voxel_object_find_color_index(voxelObject, editor.colorToEdit);
    if(treeI != -1){
        printf("tree index %d\n", treeI);
        voxel_tree_edit(&voxelObject->voxelColorTree[treeI].voxelTree, editor.voxelTreeEditor);
    }else{
        printf("tree does not exist create new tree \n");
        voxel_object_add_color_tree(voxelObject, editor.colorToEdit);
        voxel_object_edit(voxelObject, editor);
    }
    
}

*/