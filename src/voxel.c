#include "../include/voxel.h"

void init_voxel_children(Voxel* voxel){
    for(int i = 0; i < 8; i++){
        voxel->children[i] = NULL;
    }
}
int is_leaf_voxel(Voxel voxel){
    int nullCount = 0;
    for(int i = 0; i < 8; i++){
        nullCount += voxel.children[i] == NULL;
    }
    return nullCount == 8;
    
}
int get_child_index_relative_to_origin(int origin[3] , int x, int y, int z){
    int px = x >= origin[0];
    int py = y >= origin[1];
    int pz = z >= origin[2];
    return (px << 2) + (py << 1) + pz;
}
void double_voxel_positive(Voxel* voxel){
    voxel->size = voxel->size << 1;
    VoxInt offset = voxel->size >> 1;
    set_voxel_origin(voxel, offset, offset, offset);
    
}

void double_voxel_negative(Voxel* voxel){
    voxel->size = voxel->size << 1;
    VoxInt offset = voxel->size >> 1;
    set_voxel_origin(voxel, -offset, -offset, -offset);
}

int add_voxel_child_to_voxel_head(Voxel** voxel, VoxInt x, VoxInt y, VoxInt z){
    if(*voxel == NULL){
        *voxel = (Voxel*) malloc(sizeof(Voxel));
        Voxel* voxelPointer = (*voxel);
        init_voxel_children(voxelPointer);
        voxelPointer->size = 2;
        set_voxel_origin(voxelPointer, 1,1,1);
        int growthCount = 0;
        while (!is_inside_voxel(*voxelPointer, x, y, z) && growthCount < 100)
        {
            double_voxel_positive(voxelPointer);
            growthCount++;
        }
        if(growthCount == 100){
            return 0;
            printf("error when adding voxel \n");
        }
        add_child_to_voxel_node(voxelPointer, x, y, z);
        return 1;
    }

    int growthCount = 0;

    while (!is_inside_voxel(**voxel, x, y, z) && growthCount < 100){        
        Voxel* newHead = (Voxel*) malloc(sizeof(Voxel));
        newHead->size = (*voxel)->size << 1;
        newHead->origin[0] = newHead->size/2;
        newHead->origin[1] = newHead->size/2;
        newHead->origin[2] = newHead->size/2;
        init_voxel_children(newHead);
      
        newHead->children[VOXEL_CORNER_MIN] = *voxel;
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
    int voxelOrigin[3];
    int voxelSize = voxel->size;
    memcpy(voxelOrigin, voxel->origin, sizeof(unsigned int) * 3);
    int px = (x >= voxelOrigin[0]);
    int py = (y >= voxelOrigin[1]);
    int pz = (z >= voxelOrigin[2]);
    int n = (px << 2) + (py << 1) + pz;

    if(voxelSize == 2){
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
            voxel->children[n] = (Voxel*) malloc(sizeof(Voxel));
            init_voxel_children(voxel->children[n]);
            voxel->children[n]->size = voxel->size /2;

            int quarterSize = voxel->size / 4;

            set_voxel_origin(voxel->children[n],
                voxelOrigin[0] + px * quarterSize - (!px * quarterSize),
                voxelOrigin[1] + py * quarterSize - (!py * quarterSize),
                voxelOrigin[2] + pz * quarterSize - (!pz * quarterSize)
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

int remove_voxel_from_voxel_head(Voxel **head, VoxInt x, VoxInt y, VoxInt z){
    Voxel* headVox = (*head);
    if(headVox != NULL){
        int pred = headVox->size == 1 && 
            headVox->origin[0] == x &&
            headVox->origin[1] == y &&
            headVox->origin[2] == z;
        if(pred){
            free(headVox);
            headVox = NULL;
            return 1;
        }else{
            return remove_voxel_child_from_voxel(head,x,y,z);
        }
    }
    return 0;
}

void regenerate_voxel_children(Voxel *voxel){
    for(int i = 0; i < 8; i++){
        int px = (i < 4);
        int py = (i==2) || (i==3) || (i==6) || (i==7);
        int pz = (i==1) || (i==3) || (i==5) || (i==7);
        voxel->children[i] = (Voxel*) malloc(sizeof(Voxel));
        Voxel* child = voxel->children[i];
        child->size = voxel->size >> 1;
        child->origin[0] = voxel->origin[0];
        child->origin[1] = voxel->origin[1];
        child->origin[2] = voxel->origin[2];
        if(child->size == 1){
            child->origin[0] += (-1 * !px);
            child->origin[1] += (-1 * !py);
            child->origin[2] += (-1 * !pz);
        }else{
            int quarterSize = voxel->size >> 2;
            child->origin[0] += (quarterSize * px) + (-quarterSize * !px);
            child->origin[1] += (quarterSize * py) + (-quarterSize * !py);
            child->origin[2] += (quarterSize * pz) + (-quarterSize * !pz);
        }
        init_voxel_children(child);
    }
}
int remove_voxel_child_from_voxel(Voxel **voxel, VoxInt x, VoxInt y, VoxInt z){
    Voxel* voxelObj = (*voxel);
    if(voxelObj->size == 2){  
        if(is_leaf_voxel(*voxelObj)){                 
            regenerate_voxel_children(voxelObj);
        }
        int px = x >= voxelObj->origin[0];
        int py = y >= voxelObj->origin[1];
        int pz = z >= voxelObj->origin[2]; 
        int n = (px<<2) + (py<<1) + pz;
        if(voxelObj->children[n] != NULL){
            free(voxelObj->children[n]);
            voxelObj->children[n] = NULL;
         
            if(is_leaf_voxel(*voxelObj)){
                free(voxelObj);
                voxelObj = NULL;
            }
            return 1;
        }
    }else{
        int px = x >= voxelObj->origin[0];
        int py = y >= voxelObj->origin[1];
        int pz = z >= voxelObj->origin[2]; 
        int n =  (px<<2) + (py<<1) + pz;
        
     
        if(is_leaf_voxel(*voxelObj)){ 
            for(int i = 0; i < 8; i++){ 
                voxelObj->children[i] = (Voxel*) malloc(sizeof(Voxel));
                voxelObj->children[i]->size = voxelObj->size / 2;
                int quarterSize = voxelObj->size /4;
                
                voxelObj->children[i]->origin[0] = voxelObj->origin[0] + quarterSize*px + (-quarterSize*!px);
                voxelObj->children[i]->origin[1] = voxelObj->origin[1] + quarterSize*py + (-quarterSize*!py);
                voxelObj->children[i]->origin[2] = voxelObj->origin[2] + quarterSize*pz + (-quarterSize*!pz);
                
                init_voxel_children(voxelObj->children[i]);
            }

        }
        if(voxelObj->children[n] != NULL){
            remove_voxel_child_from_voxel(&voxelObj->children[n], x, y, z);
            if(is_leaf_voxel(*voxelObj)){
                free(voxelObj);
                voxelObj = NULL;
            }
            return 1;
        }

    }
    return 0;
}

int is_inside_voxel(Voxel voxel, VoxInt x, VoxInt y, VoxInt z){
    int halfSize = voxel.size >> 1;
    int minX = voxel.origin[0] - halfSize;
    int maxX = voxel.origin[0] + halfSize;
    int minY = voxel.origin[0] - halfSize;
    int maxY = voxel.origin[0] + halfSize;
    int minZ = voxel.origin[0] - halfSize;
    int maxZ = voxel.origin[0] + halfSize;
   
    return (x >= minX) && (x < maxX) &&
        (y >= minY) && (y < maxY ) &&
        (z >= minZ) && (z < maxZ);
    
}

int count_voxels(Voxel *voxel){
    if(voxel->size == 1){
        return 1;
    }
    if(is_leaf_voxel(*voxel)){
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
        (*index)++;
        return;
    }
 
    if(is_leaf_voxel(*voxel)){
        leafList[*index] = voxel;
        (*index)++;
        return;
    }

    for(int i = 0; i < 8; i++){
        if(voxel->children[i] != NULL){
            get_leaf_voxels(voxel->children[i], leafList, index);
        }
    }

}

void init_voxel_vertex(VoxelVertex* voxelVertex, float x, float y, float z, float r, float g, float b, float a){
    voxelVertex->x = x;
    voxelVertex->y = y;
    voxelVertex->z = z;
    voxelVertex->r = r;
    voxelVertex->g = g;
    voxelVertex->b = b;
    voxelVertex->a = a;
}