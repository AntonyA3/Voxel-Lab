#include <stdlib.h>

#include "../include/grid.h"
void initGridXZ(GridXZ* grid,float x, float z, float xStep, float zStep, int xCount, int zCount, GridColor color, int visibility){
    grid->x = x;
    grid->z = z;
    grid->xStep = xStep;
    grid->zStep = zStep;
    grid->xCount = xCount;
    grid->zCount = zCount;
    grid->color = color;
    grid->visible = grid_visible_true;
    glGenBuffers(1, &grid->model.vertsBuffer);
    updateGridXZModel(grid);

 
}

GridColor initGridColor(float r, float g, float b, float a){
    GridColor color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}


int gridXZVertexCount(GridXZ grid){
    return (grid.xCount + grid.zCount) * 2 + 4;
}
int updateGridXZModel(GridXZ* grid){
    int totalVerts = gridXZVertexCount(*grid);
    int index = 0;
    grid->model.verts = (GridVert*)calloc(totalVerts, sizeof(GridVert));

    if(grid->model.verts == NULL){
        printf("not enough space to allocate grid model");
        return 1;
    }

    for(int i = 0; i <= grid->xCount; i++){
        grid->model.verts[index].x = grid->x + grid->xStep * i;
        grid->model.verts[index].y = 0;
        grid->model.verts[index].z = grid->z;

        grid->model.verts[index].color = grid->color;
        memcpy(&grid->model.verts[index + 1], &grid->model.verts[index], sizeof(GridVert));
        grid->model.verts[index + 1].z = grid->z + grid->zStep * grid->zCount;
        index+=2;
    }
    for(int i = 0; i <= grid->zCount; i++){
        grid->model.verts[index].x = grid->x;
        grid->model.verts[index].y = 0;
        grid->model.verts[index].z = grid->z + grid->zStep * i;
        grid->model.verts[index].color = grid->color;
        memcpy(&grid->model.verts[index + 1], &grid->model.verts[index], sizeof(GridVert));
        grid->model.verts[index + 1].x = grid->x + grid->xStep * grid->xCount;
        index+=2;
    }

    glBindBuffer(GL_ARRAY_BUFFER, grid->model.vertsBuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GridVert) * totalVerts, grid->model.verts,GL_DYNAMIC_DRAW);
    return 0;
    
}
