typedef unsigned int VoxInt;
typedef struct Voxel
{
    unsigned int origin[3];
    unsigned int size;
    struct Voxel* children[8];
}Voxel;

int add_voxel_child_to_voxel_head(Voxel** voxel, VoxInt x, VoxInt y, VoxInt z);
int add_child_to_voxel_node(Voxel* voxel, VoxInt x, VoxInt y, VoxInt z);
int remove_voxel_child_from_voxel(Voxel** voxel, VoxInt x, VoxInt y, VoxInt z);
void get_box_from_voxel(Voxel voxel);
void set_voxel_origin(Voxel* voxel,VoxInt x,VoxInt y,VoxInt z);
int is_inside_voxel(Voxel voxel, VoxInt x, VoxInt y, VoxInt z);
int count_voxels(Voxel *voxel);
void init_children(Voxel* voxel);
void get_leaf_voxels(Voxel* voxel,Voxel** leafList, int *index);