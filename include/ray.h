#ifndef RAY_H
#define RAY_H
#include "../linmath/linmath.h"
#include "../include/voxel_octree.h"
#include "../include/voxel.h"
#include "../include/camera.h"

enum ray_hit_side{
    RAY_HIT_SIDE_LEFT,
    RAY_HIT_SIDE_RIGHT,
    RAY_HIT_SIDE_BOTTOM,
    RAY_HIT_SIDE_TOP,
    RAY_HIT_SIDE_FRONT,
    RAY_HIT_SIDE_BACK
};
enum ray_axis{
    RAY_AXIS_X,
    RAY_AXIS_Y,
    RAY_AXIS_Z
};
enum ray_hit_entity{
    RAY_HIT_Y0,
    RAY_HIT_VOXEL_TREE
};
typedef union
{   
    struct{
        float x, y, z, dx, dy, dz;
    };
    struct
    {
        vec3 origin;
        vec3 direction;
    };
}Ray;

int ray_vs_aabb(Ray ray, Aabb aabb, float *distance, int *side);
int ray_vs_y0(Ray ray, float *distance, int *side);
int ray_vs_voxel_octree(Ray ray, VoxelOctreeTree *voxelTree, float *distance, int *side);
int ray_vs_voxel(Ray ray, Voxel *voxel, float *distance, int *side);
//void ray_from_camera(Ray *ray, Camera camera, Cursor cursor);
#endif