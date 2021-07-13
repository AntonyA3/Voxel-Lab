#include "../include/grid.h"


void set_pos_grid(Grid* grid, float x, float y, float z){
    grid->pos[0] = x;
    grid->pos[1] = y;
    grid->pos[2] = z;
}



void set_i_vec_grid(Grid* grid, float x, float y, float z){
    grid->iVec[0] = x;
    grid->iVec[1] = y;
    grid->iVec[2] = z;
}

void set_j_vec_grid(Grid* grid, float x, float y, float z){
    grid->jVec[0] = x;
    grid->jVec[1] = y;
    grid->jVec[2] = z;
}

int get_vertex_count(Grid grid){
    return (grid.iCount + grid.jCount + 2) << 1;
}

void init_grid_vertex(GridVertex* gridVertex, float x, float y, float z, float r, float g, float b, float a){
    gridVertex->x = x;
    gridVertex->y = y;
    gridVertex->z = z;
    gridVertex->r = r;
    gridVertex->g = g;
    gridVertex->b = b;
    gridVertex->a = a;
}