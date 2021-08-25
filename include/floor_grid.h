#ifndef FLOOR_GRID_H
#define FLOOR_GRID_H
#include <GL/glew.h>
#include "../include/grid.h"
typedef struct FloorGrid
{
    Grid grid;
    float *vertexArray;
    GLuint vertexBuffer;  
}FloorGrid;

int floor_grid_vertex_count(FloorGrid floorGrid);
void floor_grid_generate_vertex_array(FloorGrid *floorGrid, float *vertexArray);
#endif