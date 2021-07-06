#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct VoxelNode
{
    int size;
    int midpoint[3];
    struct VoxelNode* children[8];
}VoxelNode;

typedef struct VoxelStore
{
    VoxelNode* head;
}VoxelStore;

typedef struct VoxelVertex
{
    float x,y,z;
}VoxelVertex;

typedef struct VoxelBoundingBox{
    int min[3], max[3];
}VoxelBoundingBox;

void initVoxelStore(VoxelStore* voxelStore);
int getVoxelDepthFromVoxelStore(VoxelStore* voxelStore);
int getVoxelDepthFromVoxelNode(VoxelNode* voxelNode);
void initVoxelNode(VoxelNode* voxelNode);
void getVoxelsFromVoxelStore(VoxelStore* voxelStore, void** voxels);
void getVoxelsFromVoxelNode(VoxelNode* parent, VoxelNode **voxels,int* index );
void addVoxelToVoxelStore(VoxelStore* voxelStore, int x, int y, int z);
int countVoxelsInVoxelStore(VoxelStore *voxelStore);
int countVoxelsInVoxelNode(VoxelNode *voxelNode);
int isWithinVoxelStore(VoxelStore* voxelStore, int x, int y, int z);
void getVoxelModelFromVoxelStore(VoxelStore *voxelStore, VoxelVertex **verticiesArr, unsigned int **indiciesArr);
void compressVoxelStore(VoxelStore* voxelStore);
void compressVoxelNode(VoxelNode* voxelNode);
VoxelBoundingBox getVoxelBoundingBox(VoxelNode voxelNode);
