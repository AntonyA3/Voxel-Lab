#include "../include/voxel.h"

void add2Voxels(){
    VoxelStore vStore;
    initVoxelStore(&vStore);
    addVoxelToVoxelStore(&vStore,0,0,0);
    addVoxelToVoxelStore(&vStore,1,1,1);
    printf("the voxel count %d\n", countVoxelsInVoxelStore(&vStore));

}

void add7Voxels(){
    VoxelStore vStore;
    initVoxelStore(&vStore);
    
    addVoxelToVoxelStore(&vStore,0,0,0);
    addVoxelToVoxelStore(&vStore,1,0,0);
    addVoxelToVoxelStore(&vStore,0,1,0);
    addVoxelToVoxelStore(&vStore,1,1,0);
    addVoxelToVoxelStore(&vStore,0,0,1);
    addVoxelToVoxelStore(&vStore,1,0,1);
    addVoxelToVoxelStore(&vStore,0,1,1);
    addVoxelToVoxelStore(&vStore,2,2,2);
    addVoxelToVoxelStore(&vStore,4,4,4);
    addVoxelToVoxelStore(&vStore,8,8,8);
    

    printf("the voxel count %d\n", countVoxelsInVoxelStore(&vStore));
}
int main(int argc, char const *argv[])
{
    add2Voxels();
    add7Voxels();
    return 0;
}
