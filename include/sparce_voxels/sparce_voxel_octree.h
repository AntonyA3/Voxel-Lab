#ifndef SPARCE_VOXEL_OCTREE_H
#define SPARCE_VOXEL_OCTREE_H
#include "../../include/shape.h"
#include "../../include/color.h"
#include "../../include/stack.h"
enum sparce_voxel_flags{SPARCE_VOXEL_LARGE, SPARCE_VOXEL_16X16X16,
    SPARCE_VOXEL_16X16X16_COMPRESSED
};
enum color_datat_flags{COLOR_DATA_ARRAY, COLOR_DATA_RFL};
enum voxel_edit_action{VOXEL_ADD, VOXEL_DELETE};


typedef struct ColorData
{
    int type;
}ColorData;

typedef struct ColorArray
{
    int type;
    ColorRGBA colorData[16][16][16];
}ColorArray;


typedef struct
{
    int colorCount;
    ColorRGBA color;
}ColorEncoding;

typedef struct ColorEncodingNode
{
    ColorEncoding encoding;
    struct ColorEncodingNode *nextColor;

}ColorEncodingNode;

typedef struct ColorRLE
{
    ColorEncodingNode *head;
}ColorRLE;


typedef struct SparceVoxel
{
    int type;
}SparceVoxel;

typedef struct SparceVoxelLarge
{
    int type;
    int origin[3];
    unsigned int size;
    struct SparceVoxel *child[8];
}SparceVoxelLarge;

typedef struct
{
    int type;
    int min[3];
    ColorData *colorData;
}SparceVoxel16x16x16;

typedef struct
{
    SparceVoxel *head;
}SparceVoxelOctree;

typedef struct{
    int action;
    int shapeType;
    void *shapeData;
    ColorRGBA brushColor;
}SparceVoxelOctreeEditor;


int sparce_voxel_index_is_left(int index);
int sparce_voxel_index_is_bottom(int index);
int sparce_voxel_index_is_front(int index);

Aabb voxel_large_get_aabb(SparceVoxelLarge *voxelLarge);
Aabb voxel_large_get_child_aabb(SparceVoxelLarge *voxel, int childId);

void printColorRFE(ColorEncodingNode *node);
void sparce_voxel_edit_large(SparceVoxelLarge **voxel, SparceVoxelOctreeEditor *editor);
void sparce_voxel_large_edit_size_32(SparceVoxelLarge **voxel,SparceVoxelOctreeEditor *editor);
void sparce_voxel_large_edit_default(SparceVoxelLarge **voxel, SparceVoxelOctreeEditor *editor);
void sparce_voxel_edit_16x16x16(SparceVoxel16x16x16 **voxel, SparceVoxelOctreeEditor *editor);
void sparce_voxel_octree_expand_empty(SparceVoxelOctree *octree, SparceVoxelOctreeEditor *editor);
void sparce_voxel_octree_expand_head(SparceVoxelOctree *octree, SparceVoxelOctreeEditor *editor);
void sparce_voxel_edit(SparceVoxel **voxel, SparceVoxelOctreeEditor *editor);
void sparce_voxel_octree_edit(SparceVoxelOctree *octree, SparceVoxelOctreeEditor *editor);
void sparce_voxel_octree_add(SparceVoxelOctree *octree, SparceVoxelOctreeEditor *editor);
#endif