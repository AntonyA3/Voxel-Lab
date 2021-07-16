#ifndef GRID_H
#define GRID_H

typedef struct GridVertex{
    float x, y, z;
    float r, g, b, a;
}GridVertex;

typedef struct Grid{
    float pos[3];
    float iVec[3];
    float jVec[3];
    int iCount;
    int jCount;
}Grid;


void set_pos_grid(Grid* grid, float x, float y, float z);
void set_i_vec_grid(Grid* grid, float x, float y, float z);
void set_j_vec_grid(Grid* grid, float x, float y, float z);
int get_vertex_count(Grid grid);

void init_grid_vertex(GridVertex* gridVertex, float x, float y, float z, float r, float g, float b, float a);
#endif