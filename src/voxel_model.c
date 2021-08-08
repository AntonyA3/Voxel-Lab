#include "../include/voxel_model.h"

float BOX_POSITION_ARRAY[3 * CUBE_VERTEX_COUNT] = {
    //left face
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,

    //top face
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,

    //front face
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    //bottom face
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    //right face
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,

    //back face
    1.0, -1.0, 1.0,
    1.0, 1.0, 1.0,
    -1.0, 1.0, 1.0,
    -1.0, -1.0, 1.0
};

unsigned int BOX_ELEMENT_ARRAY[36] = {
    0, 1, 2,    0, 2, 3,
    4, 5, 6,    4, 6, 7,
    8, 9, 10,    8, 10, 11,
    12, 13, 14,    12, 14, 15,
    16, 17, 18,    16, 18, 19,
    20, 21, 22,    20, 22, 23,
};

float BOX_NORMAL_ARRAY[3 * CUBE_VERTEX_COUNT] = {
    //left face
    -1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,

    //top face
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,

    //front face
    0.0, 0.0, -1.0,
    0.0, 0.0, -1.0,
    0.0, 0.0, -1.0,
    0.0, 0.0, -1.0,

    //bottom face
    0.0, -1.0, 0.0,
    0.0, -1.0, 0.0,
    0.0, -1.0, 0.0,
    0.0, -1.0, 0.0,
    //right face
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,

    //back face
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0
};


int vertex_format_size(int format){
    int vertexSize = 0;
    vertexSize += (format && VERTEX_FLAGS_POSITION) * 3;
    vertexSize += (format && VERTEX_FLAGS_NORMAL) * 3;
    vertexSize += (format && VERTEX_FLAGS_COLOR_RGBA) * 4;
    return vertexSize;

}

/*
void cube_maker(Aabb cube, int vertexFormat, float *vertexArray, unsigned int *elementArray, float *colorData, int n){
    int vertexSize = vertex_format_size(vertexFormat);
    vec3 halfExtents = {cube.w * 0.5, cube.w * 0.5, cube.d * 0.5};
    vec3 origin;
    aabb_get_centre(cube, origin);
    float verts[72];
    memcpy(verts, &BOX_POSITION_ARRAY, sizeof(float) * 72);
    int j = 0;
    for(int i = 0; i < CUBE_VERTEX_COUNT * vertexSize; i += vertexSize){
        memcpy(&vertexArray[i], &verts[j], sizeof(vec3));
        printf("j is %d\n", j);
        j += 3;
    }
    /*
    for(int i = 0; i < CUBE_VERTEX_COUNT * vertexSize; ){
        memcpy(&vertexArray[i], &BOX_POSITION_ARRAY[j], sizeof(vec3));
        //j+= 0;
        
        i += 3;
        i += 4;
        i += 3;
    }
    */
    /*

    for(int i = 0; i < CUBE_VERTEX_COUNT * 10; i++){
        printf("j is %d \n", j);
        memcpy(&vertexArray[j], &BOX_POSITION_ARRAY[i * 3], sizeof(vec3));
        i+= 10;

    }
    //memcpy(&elementArray, BOX_ELEMENT_ARRAY, sizeof(unsigned int) * 36);
    
}
*/

/*


void voxel_model_from_voxel(Voxel* voxel, int vertexFormat, float *vertexArray, unsigned int *elementArray, int *stride){

    switch (voxel->size)
    {
    case 1:
        {
            float color[4] = {0.5,1.0,0.5,1.0};
            Aabb voxBox = voxel_to_aabb(*voxel);
            float* va[72];
            unsigned int* ea[36];
            cube_maker(voxBox, 
                vertexFormat, 
                va, 
                ea, color, *stride);

            
            
            *stride += 1;
        }
        break;
    default:
        switch (voxel_is_leaf(voxel))
        {
        case 1:
            {
                
                float color[4] = {0.5,1.0,0.5,1.0};
                Aabb voxBox = voxel_to_aabb(*voxel);
                cube_maker(voxBox, 
                    vertexFormat, 
                    &vertexArray[*stride * CUBE_VERTEX_COUNT  * vertex_format_size(vertexFormat)], 
                    &elementArray[*stride * CUBE_ELEMENT_COUNT], color ,*stride
                );
                *stride += 1;
                
            }
            break;
        default:
            {
                for(int i= 0; i < 8; i++){
                    if(voxel->child[i] != NULL){
                        voxel_model_from_voxel(voxel->child[i], vertexFormat, vertexArray, elementArray, stride);
                    }
                }
            }
            break;
        }
        break;
    }
}

*/

void voxel_model_from_cube(Aabb aabb, float *vertexArray, unsigned int *elementArray, int n, float *colorData){

    int j = 0;
    int k = 0;
    vec3 origin;
    vec3 halfExtents = {aabb.w * 0.5, aabb.h * 0.5, aabb.d * 0.5};
    aabb_get_centre(aabb, origin);
    for(int i = 0; i < 240;){    
        memcpy(&vertexArray[i], &BOX_POSITION_ARRAY[j], sizeof(float) * 3);
        for(int t = 0; t < 3; t++){
            vertexArray[i +t] *= halfExtents[t];
        }
        vec3_add(&vertexArray[i], &vertexArray[i], origin);
        j+=3;
        i+=3;
        memcpy(&vertexArray[i], colorData, sizeof(vec4));
        i+=4;

        memcpy(&vertexArray[i], &BOX_NORMAL_ARRAY[k], sizeof(vec3));
        k+=3;
        i+=3;
    }
    memcpy(elementArray, BOX_ELEMENT_ARRAY, sizeof(unsigned int) * 36);
    for(int i = 0; i < 36; i++){
        elementArray[i] += n * CUBE_VERTEX_COUNT;
    }

}

void voxel_model_update_from_voxel(Voxel *voxel, float *vertexArray, unsigned int *elementArray, int *stride){ 
    switch (voxel->size)
    {
    case 1:
        {
            vec4 green = {0.0,1.0,0.0,1.0};
            Aabb aabb = voxel_to_aabb(*voxel);
            voxel_model_from_cube(aabb, &vertexArray[*stride * 240], &elementArray[*stride * 36], *stride, green);
            *stride += 1;
        }
        break;
    
    default:
        switch (voxel_is_leaf(voxel))
        {
        case 1:
            {
                vec4 green = {0.0,1.0,0.0,1.0};

                Aabb aabb = voxel_to_aabb(*voxel);
                voxel_model_from_cube(aabb, &vertexArray[*stride * 240], &elementArray[*stride * 36], *stride,green);
                *stride += 1;
            }
            break;
        default:
            for(int i = 0; i < 8; i++){
                if(voxel->child[i] != NULL){
                    voxel_model_update_from_voxel(voxel->child[i], vertexArray, elementArray, stride);
                }
            }
            break;
        }
        break;
    }
}

void voxel_model_update_from_tree(VoxelModel *voxelModel, VoxelTree *voxelTree, int format){
    int vertexSize = 10;
    int vertexArraySize = sizeof(float) * vertexSize * CUBE_VERTEX_COUNT * voxelTree->voxelCount;
    int elementArraySize = sizeof(unsigned int) * CUBE_ELEMENT_COUNT * voxelTree->voxelCount;
    voxelModel->vertexArray = (float*) realloc(voxelModel->vertexArray, vertexArraySize);
    voxelModel->elementArray = (unsigned int*) realloc(voxelModel->elementArray, elementArraySize);
    
    if(voxelModel->vertexArray && voxelModel->elementArray){
        int stride = 0;
        voxel_model_update_from_voxel(voxelTree->head, voxelModel->vertexArray, voxelModel->elementArray, &stride);
    }

    glBindBuffer(GL_ARRAY_BUFFER, voxelModel->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexArraySize, voxelModel->vertexArray, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelModel->elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementArraySize, voxelModel->elementArray, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  
    
    
}

  
