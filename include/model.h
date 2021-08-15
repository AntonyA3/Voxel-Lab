#ifndef MODEL_H
#define MODEL_H
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "../include/aabb.h"
#include "../include/grid.h"
#include "../include/voxel_tree.h"
#include "../include/voxel.h"
#include "../include/vertex.h"

enum model_type{MODEL_TYPE_GRID, MODEL_TYPE_VOXEL_MODEL, MODEL_TYPE_AABB, MODEL_TYPE_TEST_TRI};
enum model_draw_method{MODEL_DRAW_ELEMENTS, MODEL_DRAW_VERTICIES_LINES, MODEL_DRAW_VERTICIES};
typedef struct
{
    int type;
    float *vertexArray;
    unsigned int *elementArray;
    GLuint vertexBuffer;
    GLuint elementBuffer;
    int vertexCount;
    int elementCount;
}Model;

void model_generate_aabb_element_array(unsigned int *elementsArray, int n);
void model_generate_grid_vertex_array(float *vertexArray, Grid grid, float* color, int vertexFormat);
void model_generate_aabb_vertex_array(float *vertexArray, Aabb aabb, float *color, int vertexFormat);
void model_generate(int modelType, Model* model, 
    void *positionData, void *colorData,
    int vertexFormat, int usage
);

void model_draw(Model *model, int vertexFormat, int method, int vertexCount);

#endif
