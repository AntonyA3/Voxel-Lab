#ifndef GRID_XZ_H
#define GRID_XZ_H
typedef struct
{
    float x, z;
    float sizeX, sizeZ;
    int xCount, zCount;
}GridXZ;
int grid_xz_get_vertex_count(GridXZ grid);
#endif