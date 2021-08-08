#ifndef VOXEL_MODEL_H
#define VOXEL_MODEL_H

#define CUBE_VERTEX_COUNT 24
#define CUBE_ELEMENT_COUNT 36

#include <GL/glew.h>
#include "../linmath/linmath.h"
#include "../include/voxel_tree.h"
#include "../include/vertex_flags.h"

typedef struct{
    float *vertexArray;
    unsigned int *elementArray;
    GLuint vertexBuffer, elementBuffer;
}VoxelModel;

extern float BOX_POSITION_ARRAY[3 * CUBE_VERTEX_COUNT];
extern float BOX_NORMAL_ARRAY[3 * CUBE_VERTEX_COUNT];

extern unsigned int BOX_ELEMENT_ARRAY[36];

void voxel_model_from_cube(Aabb aabb, float *vertexArray, unsigned int *elementArray, int n,float *colorData);
void voxel_model_update_from_voxel(Voxel *voxel, float *vertexArray, unsigned int *elementArray, int *stride); 
void voxel_model_update_from_tree(VoxelModel *voxelModel, VoxelTree *voxelTree, int format);
void voxel_model_manipulate_manipulator(VoxelModel *voxelModel,VoxelTree *voxelTree, float deltaTime, int buttonState);
#endif