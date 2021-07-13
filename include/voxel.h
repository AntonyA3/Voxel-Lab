#include <stdlib.h>
#include <stdio.h>
#include <string.h>
enum voxel_corner{VOXEL_CORNER_MIN = 0, VOXEL_CORNER_MAX = 7};
typedef unsigned int VoxInt;

typedef struct Voxel
{
    unsigned int origin[3];
    unsigned int size;
    struct Voxel* children[8];
}Voxel;

typedef struct VoxelVertex{
    float x, y, z;
    float r, g, b, a;
}VoxelVertex;
int is_large_child_voxel(Voxel voxel);
int get_child_index_relative_to_origin(int origin[3] , int x, int y, int z);
int add_voxel_child_to_voxel_head(Voxel** voxel, VoxInt x, VoxInt y, VoxInt z);
int add_child_to_voxel_node(Voxel* voxel, VoxInt x, VoxInt y, VoxInt z);

int remove_voxel_child_from_voxel(Voxel **voxel, VoxInt x, VoxInt y, VoxInt z);
int remove_voxel_from_voxel_head(Voxel **head, VoxInt x, VoxInt y, VoxInt z);

void get_box_from_voxel(Voxel voxel);
void set_voxel_origin(Voxel* voxel,VoxInt x,VoxInt y,VoxInt z);
int is_inside_voxel(Voxel voxel, VoxInt x, VoxInt y, VoxInt z);
int count_voxels(Voxel *voxel);
void init_children(Voxel* voxel);
void get_leaf_voxels(Voxel* voxel,Voxel** leafList, int *index);
void init_voxel_vertex(VoxelVertex* voxelVertex, float x, float y, float z, float r, float g, float b, float a);