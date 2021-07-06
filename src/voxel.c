#include "../include/voxel.h"


void initVoxelStore(VoxelStore* voxelStore){
    voxelStore->head = NULL;
}

void initVoxelNode(VoxelNode* voxelNode){
    for (int i = 0; i < 8; i++)
    {
        voxelNode->children[i] = NULL;
    }  
}
int getVoxelDepthFromVoxelStore(VoxelStore* voxelStore){
    if(voxelStore->head == NULL){
        return 0;
    }
    return getVoxelDepthFromVoxelNode(voxelStore->head);
}

int getVoxelDepthFromVoxelNode(VoxelNode* voxelNode){
    int maxDepth = 0;
    for(int i = 0; i < 8; i++){
        if(voxelNode->children[i] != NULL){
            int d = getVoxelDepthFromVoxelNode(voxelNode->children[i]);
            maxDepth = maxDepth * (maxDepth >= d) + d * (d > maxDepth);
        }
    }
    return 1 + maxDepth;

}

void addVoxelToVoxelStore(VoxelStore* voxelStore, int x, int y, int z){
    if(x < 0 || y < 0 || z < 0){
        return;
    }
    if(voxelStore->head == NULL){
        voxelStore->head = (VoxelNode*) malloc(sizeof(VoxelNode));
        initVoxelNode(voxelStore->head);
        voxelStore->head->size = 2;
        voxelStore->head->midpoint[0] = 1;
        voxelStore->head->midpoint[1] = 1;
        voxelStore->head->midpoint[2] = 1;
        int safety = 0;
        while (!isWithinVoxelStore(voxelStore, x,y,z) && safety < 100){
            safety++;
            VoxelNode* newHead = (VoxelNode*) malloc(sizeof(VoxelNode));
            initVoxelNode(newHead);                
            newHead->size = voxelStore->head->size * 2;
        
            newHead->midpoint[0] = voxelStore->head->midpoint[0] * 2;
            newHead->midpoint[1] = voxelStore->head->midpoint[1] * 2;
            newHead->midpoint[2] = voxelStore->head->midpoint[2] * 2; 
        
            newHead->children[0] = voxelStore->head;
            voxelStore->head = newHead;
        }
        addVoxelToVoxelNode(voxelStore->head, x, y, z);
    }else{
        int safety = 0;
        while (!isWithinVoxelStore(voxelStore, x,y,z) && safety < 100)
        {
            safety += 1;
            
            VoxelNode* newHead = (VoxelNode*) malloc(sizeof(VoxelNode));
            initVoxelNode(newHead);                
            newHead->size = voxelStore->head->size * 2;
            if(newHead->midpoint[0] == 0){
                newHead->midpoint[0] = 1;
                newHead->midpoint[1] = 1;
                newHead->midpoint[2] = 1; 
            }else{
                newHead->midpoint[0] = voxelStore->head->midpoint[0] * 2;
                newHead->midpoint[1] = voxelStore->head->midpoint[1] * 2;
                newHead->midpoint[2] = voxelStore->head->midpoint[2] * 2; 
            }
            
            
            newHead->children[0] = voxelStore->head;
            voxelStore->head = newHead;
        }
        addVoxelToVoxelNode(voxelStore->head, x, y, z);
    }
}

void addVoxelToVoxelNode(VoxelNode* voxelNode, int x, int y, int z){
    int px = x >= voxelNode->midpoint[0];
    int py = y >= voxelNode->midpoint[1];
    int pz = z >= voxelNode->midpoint[2];  
    int n = 4 * px + 2 * py + pz;
    
    if(voxelNode->size == 2){
        if(voxelNode->children[n] == NULL){
            VoxelNode* child =(VoxelNode*) malloc(sizeof(VoxelNode));
            initVoxelNode(child);
            child->midpoint[0] = x;
            child->midpoint[1] = y;
            child->midpoint[2] = z;
            child->size = 1;
            voxelNode->children[n] = child;
        }
        
    }else{
        if(voxelNode->children[n] == NULL){
            VoxelNode* child = (VoxelNode*) malloc(sizeof(VoxelNode));
            initVoxelNode(child);
            int qs = voxelNode->size / 4;
            child->size = voxelNode->size / 2;
            switch (n)
            {
            case 7:
                child->midpoint[0] = voxelNode->midpoint[0] + qs;
                child->midpoint[1] = voxelNode->midpoint[1] + qs;
                child->midpoint[2] = voxelNode->midpoint[2] + qs;
                break;
            case 6:
                child->midpoint[0] = voxelNode->midpoint[0] + qs;
                child->midpoint[1] = voxelNode->midpoint[1] + qs;
                child->midpoint[2] = voxelNode->midpoint[2] - qs;
                break;
            case 5:
                child->midpoint[0] = voxelNode->midpoint[0] + qs;
                child->midpoint[1] = voxelNode->midpoint[1] - qs;
                child->midpoint[2] = voxelNode->midpoint[2] + qs;
                break;
            case 4:
                child->midpoint[0] = voxelNode->midpoint[0] + qs;
                child->midpoint[1] = voxelNode->midpoint[1] - qs;
                child->midpoint[2] = voxelNode->midpoint[2] - qs;
                break;
            case 3:
                child->midpoint[0] = voxelNode->midpoint[0] - qs;
                child->midpoint[1] = voxelNode->midpoint[1] + qs;
                child->midpoint[2] = voxelNode->midpoint[2] + qs;
                break;
            case 2:
                child->midpoint[0] = voxelNode->midpoint[0] - qs;
                child->midpoint[1] = voxelNode->midpoint[1] + qs;
                child->midpoint[2] = voxelNode->midpoint[2] - qs;
                break;
            case 1:
                child->midpoint[0] = voxelNode->midpoint[0] - qs;
                child->midpoint[1] = voxelNode->midpoint[1] - qs;
                child->midpoint[2] = voxelNode->midpoint[2] + qs;
                break;
            case 0: 
                child->midpoint[0] = voxelNode->midpoint[0] - qs;
                child->midpoint[1] = voxelNode->midpoint[1] - qs;
                child->midpoint[2] = voxelNode->midpoint[2] - qs;
                break;
            }
            voxelNode->children[n] = child;
        } 
        addVoxelToVoxelNode(voxelNode->children[n],x ,y ,z);
        
    }
}

VoxelBoundingBox getVoxelBoundingBox(VoxelNode voxelNode){
    VoxelBoundingBox box;
    if(voxelNode.size == 1){
        box.min[0] = voxelNode.midpoint[0];
        box.min[1] = voxelNode.midpoint[1];
        box.min[2] = voxelNode.midpoint[2];

        box.max[0] = voxelNode.midpoint[0] + 1;
        box.max[1] = voxelNode.midpoint[1] + 1;
        box.max[2] = voxelNode.midpoint[2] + 1;
        return box;
    }
    int halfSize = voxelNode.size >> 1;
    box.min[0] = voxelNode.midpoint[0] - halfSize;
    box.min[1] = voxelNode.midpoint[1] - halfSize;
    box.min[2] = voxelNode.midpoint[2] - halfSize;

    box.max[0] = voxelNode.midpoint[0] + halfSize;
    box.max[1] = voxelNode.midpoint[1] + halfSize;
    box.max[2] = voxelNode.midpoint[2] + halfSize;
    return box;
}

int countVoxelsInVoxelStore(VoxelStore *voxelStore){
    if(voxelStore->head == NULL){
        return 0;
    }
    return countVoxelsInVoxelNode(voxelStore->head);
}

int countVoxelsInVoxelNode(VoxelNode *voxelNode){
    int nullCount = 0;
    for(int i= 0; i < 8; i++){
        nullCount += (voxelNode->children[i] == NULL);
    }
    if(nullCount == 8){
        return 1;
    }

    int childCount = 0;
    for (int i = 0; i < 8; i++)
    {
        if(voxelNode->children[i] != NULL){
            childCount += countVoxelsInVoxelNode(voxelNode->children[i]);
        }
    }
    return childCount;
}

int isWithinVoxelStore(VoxelStore* voxelStore, int x, int y, int z){
    if(voxelStore->head == NULL){
        return 0;
    }
    if(voxelStore->head->size == 1){
        return 0;
    }
    VoxelNode* head = voxelStore->head;
    int hs = head->size/2;
    int minX = head->midpoint[0] -hs;
    int maxX = head->midpoint[0] + hs;
    int minY = head->midpoint[1] -hs;
    int maxY = head->midpoint[1] + hs;
    int minZ = head->midpoint[2] -hs;
    int maxZ = head->midpoint[2] + hs;

    return x >= minX && x < maxX && 
        y >= minY && y < maxY &&
        z >= minZ && z < maxZ;
}

void getVoxelsFromVoxelStore(VoxelStore* voxelStore, void** voxels){
    int index = 0;
    getVoxelsFromVoxelNode(voxelStore->head, voxels, &index);
}

void getVoxelsFromVoxelNode(VoxelNode* parent,  VoxelNode **voxels ,int* index ){
    int nullCount = 0;
    for(int i = 0; i < 8; i++){
        nullCount += (parent->children[i] == NULL);
    }
    if(nullCount == 8){
        voxels[*index] = parent;
        *index += 1;
    }else{
        for (int i = 0; i < 8; i++)
        {
            if(parent->children[i] != NULL){
                getVoxelsFromVoxelNode(parent->children[i], voxels, index);
            }
        }
        
    }

}
void compressVoxelNode(VoxelNode* voxelNode){
    if(voxelNode->size == 2){
        int childCount = 0;
        for(int i = 0; i < 8; i++){
            childCount += voxelNode->children[i] != NULL;
        }
        if(childCount == 8){
            for(int i = 0; i < 8; i++){
                free(voxelNode->children[i]);
                voxelNode->children[i] = NULL;
            }
        }
    }else if(voxelNode->size > 2){
        for(int i = 0; i < 8; i++){
            if(voxelNode->children[i] != NULL){
                compressVoxelNode(voxelNode->children[i]);
            }
        }
        int childCount = 0;
        for(int i = 0; i < 8; i++){   
            childCount += voxelNode->children[i] != NULL;
        }
        
        if(childCount == 8){
            int nullCount = 0;
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    nullCount += voxelNode->children[i]->children[j] == NULL;
                }
            }
            if(nullCount == 64){
                for(int i = 0; i < 8; i++){
                    free(voxelNode->children[i]);
                    voxelNode->children[i] = NULL;
                }
            }
        }
    }
}

void compressVoxelStore(VoxelStore* voxelStore){
    compressVoxelNode(voxelStore->head);
}


void getVoxelModelFromVoxelStore(VoxelStore *voxelStore, VoxelVertex **verticies, unsigned int **indicies){
    

    int voxelCount = countVoxelsInVoxelStore(voxelStore);
    int vertexIndex = 0;
    int indiciesIndex = 0;
    unsigned int cubeIndex = 0;
    *verticies = (VoxelVertex*) calloc(8 * voxelCount,sizeof(VoxelVertex));
    *indicies = (unsigned int*) calloc(12 * 3 * voxelCount,sizeof(unsigned int));
    VoxelNode *voxels[voxelCount];
    getVoxelsFromVoxelStore(voxelStore, voxels);
    

    for(int i = 0; i < voxelCount; i++){
        VoxelNode *vn = voxels[i];

        int x = vn->midpoint[0];
        int y = vn->midpoint[1];
        int z = vn->midpoint[2];        
        int size = vn->size;

        if(vn->size != 1){
            int hs = size / 2;
            x -= hs;
            y -= hs;
            z -= hs;
        }
        (*verticies)[vertexIndex].x = x;
        (*verticies)[vertexIndex].y = y;
        (*verticies)[vertexIndex].z = z;
        vertexIndex++;

        (*verticies)[vertexIndex].x = x;
        (*verticies)[vertexIndex].y = y + size;
        (*verticies)[vertexIndex].z = z;
        vertexIndex++;

        (*verticies)[vertexIndex].x = x + size;
        (*verticies)[vertexIndex].y = y + size;
        (*verticies)[vertexIndex].z = z;
        vertexIndex++;

        (*verticies)[vertexIndex].x = x + size;
        (*verticies)[vertexIndex].y = y;
        (*verticies)[vertexIndex].z = z;
        vertexIndex++;

        (*verticies)[vertexIndex].x = x;
        (*verticies)[vertexIndex].y = y;
        (*verticies)[vertexIndex].z = z + size;
        vertexIndex++;

        (*verticies)[vertexIndex].x = x;
        (*verticies)[vertexIndex].y = y + size;
        (*verticies)[vertexIndex].z = z + size;
        vertexIndex++;

        (*verticies)[vertexIndex].x = x + size;
        (*verticies)[vertexIndex].y = y + size;
        (*verticies)[vertexIndex].z = z + size;
        vertexIndex++;

        (*verticies)[vertexIndex].x = x + size;
        (*verticies)[vertexIndex].y = y;
        (*verticies)[vertexIndex].z = z + size;
        vertexIndex++;

        //Face 0
        (*indicies)[indiciesIndex] = cubeIndex + 4;
        (*indicies)[indiciesIndex + 1] = cubeIndex + 5;
        (*indicies)[indiciesIndex + 2] = cubeIndex + 0;

        (*indicies)[indiciesIndex + 3] = cubeIndex + 5;
        (*indicies)[indiciesIndex + 4] = cubeIndex + 1;
        (*indicies)[indiciesIndex + 5] = cubeIndex + 0;
        //Face 1
        (*indicies)[indiciesIndex + 6] = cubeIndex + 0;
        (*indicies)[indiciesIndex + 7] = cubeIndex + 1;
        (*indicies)[indiciesIndex + 8] = cubeIndex + 3;

        (*indicies)[indiciesIndex + 9] = cubeIndex + 1;
        (*indicies)[indiciesIndex + 10] = cubeIndex + 2;
        (*indicies)[indiciesIndex + 11] = cubeIndex + 3;
        
        //Face 2
        (*indicies)[indiciesIndex + 12] = cubeIndex + 1;
        (*indicies)[indiciesIndex + 13] = cubeIndex + 5;
        (*indicies)[indiciesIndex + 14] = cubeIndex + 2;

        (*indicies)[indiciesIndex + 15] = cubeIndex + 5;
        (*indicies)[indiciesIndex + 16] = cubeIndex + 6;
        (*indicies)[indiciesIndex + 17] = cubeIndex + 2;

        //Face 3
        (*indicies)[indiciesIndex + 18] = cubeIndex + 4;
        (*indicies)[indiciesIndex + 19] = cubeIndex + 0;
        (*indicies)[indiciesIndex + 20] = cubeIndex + 7;

        (*indicies)[indiciesIndex + 21] = cubeIndex + 0;
        (*indicies)[indiciesIndex + 22] = cubeIndex + 3;
        (*indicies)[indiciesIndex + 23] = cubeIndex + 7;

        //Face 4
        (*indicies)[indiciesIndex + 24] = cubeIndex + 3;
        (*indicies)[indiciesIndex + 25] = cubeIndex + 2;
        (*indicies)[indiciesIndex + 26] = cubeIndex + 7;

        (*indicies)[indiciesIndex + 27] = cubeIndex + 2;
        (*indicies)[indiciesIndex + 28] = cubeIndex + 6;
        (*indicies)[indiciesIndex + 29] = cubeIndex + 7;

        //Face 5
        (*indicies)[indiciesIndex + 30] = cubeIndex + 7;
        (*indicies)[indiciesIndex + 31] = cubeIndex + 6;
        (*indicies)[indiciesIndex + 32] = cubeIndex + 4;

        (*indicies)[indiciesIndex + 33] = cubeIndex + 6;
        (*indicies)[indiciesIndex + 34] = cubeIndex + 5;
        (*indicies)[indiciesIndex + 35] = cubeIndex + 4;
        indiciesIndex += 36;
        cubeIndex += 8;
    }

}

