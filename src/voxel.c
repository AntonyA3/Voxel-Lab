#include <stdio.h>
#include <stdlib.h>
#include "../include/voxel.h"

void init_voxel_children(Voxel* voxel){
    for(int i = 0; i < 8; i++){
        voxel->children[i] = NULL;
    }
}

int add_voxel_child_to_voxel_head(Voxel** voxel, VoxInt x, VoxInt y, VoxInt z){
    if(*voxel == NULL){
        *voxel = (Voxel*) malloc(sizeof(Voxel));
        init_voxel_children(*voxel);
        (*voxel)->size = 2;
        set_voxel_origin(*voxel, 1,1,1);
        int growthCount = 0;
        while (!is_inside_voxel(**voxel, x, y, z) && growthCount < 100)
        {
            (*voxel)->size = (*voxel)->size << 1;
            VoxInt mid = (*voxel)->size >> 1;
            set_voxel_origin (*voxel, mid,mid,mid);
            growthCount++;
        }
        if(growthCount == 100){
            return 0;
            printf("error when adding voxel \n");
        }
        add_child_to_voxel_node(*voxel, x, y, z);
        return 1;
    }

    int growthCount = 0;
    while (!is_inside_voxel(**voxel, x, y, z) && growthCount < 100){        
        Voxel* newHead = (Voxel*) malloc(sizeof(Voxel));
        init_voxel_children(newHead);
        newHead->size = (*voxel)->size << 1;        
        VoxInt mid = (*voxel)->size;
        set_voxel_origin(newHead, mid, mid, mid);

        newHead->children[0] = *voxel;
        for(int i = 1; i < 8; i++){
            newHead->children[i] = NULL;
        }
        *voxel = newHead;
        growthCount++;
    }
    if(growthCount == 100){
        return 0;
        printf("error when adding voxel \n");
    }
    return add_child_to_voxel_node(*voxel, x, y, z);
}
int add_child_to_voxel_node(Voxel* voxel, VoxInt x, VoxInt y, VoxInt z){
    int px = (x >= voxel->origin[0]);
    int py = (y >= voxel->origin[1]);
    int pz = (z >= voxel->origin[2]);
    int n = 4 * px + 2 * py + pz;
    if(voxel->size == 2){
        if(voxel->children[n] == NULL){
            voxel->children[n] = (Voxel*) malloc(sizeof(Voxel));
            init_voxel_children(voxel->children[n]);
            set_voxel_origin(voxel->children[n], x, y, z);
            voxel->children[n]->size = 1;
            return 1;
        }
        return 0;
        
    }else{
        if(voxel->children[n] == NULL){
            int quarterSize = voxel->size >> 2;
            voxel->children[n] = (Voxel*) malloc(sizeof(Voxel));
            init_voxel_children(voxel->children[n]);
            voxel->children[n]->size = voxel->size >> 1;
            set_voxel_origin(voxel->children[n],
                voxel->origin[0] + px * quarterSize - (!px * quarterSize),
                voxel->origin[1] + py * quarterSize - (!py * quarterSize),
                voxel->origin[2] + pz * quarterSize - (!pz * quarterSize)
            );
        }
        return add_child_to_voxel_node(voxel->children[n],x,y,z);
    }

}

void set_voxel_origin(Voxel* voxel, VoxInt x, VoxInt y, VoxInt z){
    voxel->origin[0] = x;
    voxel->origin[1] = y;
    voxel->origin[2] = z;
}

int remove_voxel_child_from_voxel(Voxel** voxel, VoxInt x, VoxInt y, VoxInt z){
    printf("not yet implemented\n");
    return 0;
}

int is_inside_voxel(Voxel voxel, VoxInt x, VoxInt y, VoxInt z){
    int halfVoxelSize = voxel.size >> 1;
    int dx = x - voxel.origin[0];
    int dy = y - voxel.origin[1];
    int dz = z - voxel.origin[2];
    return (dx >= -halfVoxelSize) && (dx < halfVoxelSize) &&
        (dy >= -halfVoxelSize) && (dy < halfVoxelSize) &&
        (dz >= -halfVoxelSize) && (dz < halfVoxelSize);

}

int count_voxels(Voxel *voxel){
    if(voxel->size == 1){
        return 1;
    }
    int nullCount = 0;
    for(int i = 0; i < 8; i++){
        nullCount += voxel->children[i] == NULL;
    }
    if(nullCount == 8){
        return 1;
    }
    int totalVoxelCount = 0;
    for(int i = 0; i < 8; i++){
        if(voxel->children[i] != NULL){
            totalVoxelCount += count_voxels(voxel->children[i]);
        }
        
    }
    return totalVoxelCount;
}

void get_leaf_voxels(Voxel* voxel,Voxel** leafList, int *index){
    if(voxel->size == 1){
        leafList[*index] = voxel;
        *index+= 1;
        return;
    }
    int nullCount = 0;
    for(int i = 0; i < 8; i++){
        nullCount += voxel->children[i] == NULL;
    }
    if(nullCount == 8){
        leafList[*index] = voxel;
        *index+=1;
        return;
    }

    for(int i = 0; i < 8; i++){
        if(voxel->children[i] != NULL){
            get_leaf_voxels(voxel->children[i], leafList, index);
        }
    }

}