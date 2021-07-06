#include <stdio.h>
#include <GL/glew.h>
#include "linmath.h"

typedef struct GridColor{
    float r,g,b,a;
}GridColor;
enum grid_visible{
    grid_visible_false,
    grid_visible_true
};

typedef struct GridVert
{
    float x;
    float y;
    float z;
    GridColor color;
}GridVert;

typedef struct GridModel
{
    GridVert* verts;
    GLuint vertsBuffer;
    
}GridModel;

typedef struct GridXZ{
    float x, z;
    float xStep, zStep;
    int xCount, zCount;
    int visible;
    GridColor color;
    GridModel model;

}GridXZ;

void initGridXZ(GridXZ* grid,float x, float z, float xStep, float zStep, int xCount, int zCount, GridColor color, int visibility);
GridColor initGridColor(float r, float g, float b, float a);
int updateGridXZModel(GridXZ* grid);
int gridXZVertexCount(GridXZ grid);