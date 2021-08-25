#include "../include/model.h"

void model_append_color(float **vertexArray, float* color, int vertexFormat){
    if((vertexFormat & VERTEX_RGBA) == VERTEX_RGBA){
        memcpy(*vertexArray, color, sizeof(vec4));
        *vertexArray += VERTEX_RGBA_SIZE;  
    }else if((vertexFormat & VERTEX_RGB) == VERTEX_RGB){
        memcpy(*vertexArray, color, sizeof(vec3));
        *vertexArray += VERTEX_RGB_SIZE;  
    }
}

void model_generate_aabb_vertex_array(float *vertexArray, Aabb aabb, float *color, int vertexFormat){
    float *aabbVertexRef = AABB_VERTEX_ARRAY;
    float *aabbNormalRef = AABB_NORMAL_ARRAY;
    vec3 origin, halfExtents;
    aabb_get_origin(aabb, origin);
    vec3_scale(halfExtents, aabb.size, 0.5f);
    
    for(int i = 0; i < VERTICIES_PER_CUBE; i++){
        if((vertexFormat & VERTEX_POSITION) == VERTEX_POSITION){
            memcpy(vertexArray, aabbVertexRef, sizeof(vec3));
            vertexArray[0] = vertexArray[0] * halfExtents[0] + origin[0];
            vertexArray[1] = vertexArray[1] * halfExtents[1] + origin[1];
            vertexArray[2] = vertexArray[2] * halfExtents[2] + origin[2];
            vertexArray += VERTEX_POSITION_SIZE;  
            aabbVertexRef += VERTEX_POSITION_SIZE;
        }
        model_append_color(&vertexArray, color, vertexFormat);  

        if((vertexFormat & VERTEX_NORMAL) == VERTEX_NORMAL){
            memcpy(vertexArray, aabbNormalRef, sizeof(vec3));
            vertexArray += VERTEX_NORMAL_SIZE;
            aabbNormalRef += VERTEX_NORMAL_SIZE;
        } 
    }
}
/*
void model_generate_voxel_model(float *vertexArray, unsigned int *elementsArray, 
    Voxel *voxel, float *color, int vertexFormat, int *n
){
    switch (voxel->size)
    {
    case 1:
        model_generate_aabb_vertex_array(
            &vertexArray[(*n) * VERTICIES_PER_CUBE * vertex_size(vertexFormat)],
            voxel_to_aabb(*voxel), color, vertexFormat
        );
        model_generate_aabb_element_array(
            &elementsArray[(*n) * ELEMENTS_PER_CUBE], 
            *n
        );
        *n += 1;
        break;
    default:
        
        switch (voxel_is_leaf(voxel))
        {
        case 1:
            {
                model_generate_aabb_vertex_array(
                    &vertexArray[(*n) * VERTICIES_PER_CUBE * vertex_size(vertexFormat)],
                    voxel_to_aabb(*voxel), color, vertexFormat
                );
                model_generate_aabb_element_array(
                    &elementsArray[(*n) * ELEMENTS_PER_CUBE], 
                    *n
                );
                *n += 1;
            }
            break;
        default:
            for(int i = 0; i < VOXEL_CHILD_COUNT; i++){
                if(voxel->child[i] != NULL){
                    model_generate_voxel_model(vertexArray, elementsArray, voxel->child[i], color, vertexFormat, n);
                }
            }
            break;
        }
        break;
    }
}
*/

void model_generate_aabb_element_array(unsigned int *elementsArray, int n){
    memcpy(elementsArray, AABB_ELEMENTS_ARRAY, sizeof(unsigned int) * ELEMENTS_PER_CUBE);

    for(int i = 0; i < ELEMENTS_PER_CUBE; i++){
        elementsArray[i] += n * VERTICIES_PER_CUBE; 
    }
}

void model_generate_grid_vertex_array(float *vertexArray, Grid grid, float* color, int vertexFormat){
    vec3 horLength;
    vec3 vertLength; 
    vec3 gridPoint;   
    vec3_scale(vertLength, grid.vectorV, grid.vCount);
    vec3_scale(horLength, grid.vectorH, grid.hCount);
    memcpy(gridPoint, grid.min, sizeof(vec3));
    for(int h = 0; h <= grid.hCount; h++){
        if((vertexFormat & VERTEX_POSITION) == VERTEX_POSITION){
            memcpy(vertexArray, gridPoint,sizeof(vec3));
            vertexArray += VERTEX_POSITION_SIZE;
        }  
        model_append_color(&vertexArray, color, vertexFormat);
        vec3_add(gridPoint, gridPoint, vertLength);
        if((vertexFormat & VERTEX_POSITION) == VERTEX_POSITION){
            memcpy(vertexArray, gridPoint, sizeof(vec3));
            vertexArray += VERTEX_POSITION_SIZE;
        }
        model_append_color(&vertexArray, color, vertexFormat);
        vec3_sub(gridPoint, gridPoint, vertLength);
        vec3_add(gridPoint, gridPoint,grid.vectorH);
    }
    memcpy(gridPoint, grid.min, sizeof(vec3));
    for(int v = 0; v <= grid.vCount; v++){
        if((vertexFormat & VERTEX_POSITION) == VERTEX_POSITION){
            memcpy(vertexArray, gridPoint,sizeof(vec3));
            vertexArray += VERTEX_POSITION_SIZE;
        }  
        model_append_color(&vertexArray, color, vertexFormat);
        vec3_add(gridPoint, gridPoint, horLength);
        if((vertexFormat & VERTEX_POSITION) == VERTEX_POSITION){
            memcpy(vertexArray, gridPoint, sizeof(vec3));
            vertexArray += VERTEX_POSITION_SIZE;
        }
        model_append_color(&vertexArray, color, vertexFormat);

        vec3_sub(gridPoint, gridPoint, horLength);
        vec3_add(gridPoint, gridPoint,grid.vectorV);
    }
}

void model_generate(int modelType, Model* model, 
    void *positionData, void *colorData,
    int vertexFormat, int usage
){
    switch (modelType)
    {
    case MODEL_TYPE_VOXEL_MODEL:
        {
            /*
            VoxelTree *voxelTree = (VoxelTree*)positionData;
            int n = 0;
            int totalSize = vertex_size(vertexFormat) * sizeof(float) * VERTICIES_PER_CUBE * voxelTree->voxelCount;
            model->vertexArray = (float*) realloc(model->vertexArray, totalSize);
            model->elementArray = (unsigned int*) realloc(model->elementArray, ELEMENTS_PER_CUBE * voxelTree->voxelCount * sizeof(unsigned int));

            model_generate_voxel_model(model->vertexArray, model->elementArray,
                voxelTree->head, colorData, vertexFormat, &n
            );

            glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, totalSize, model->vertexArray, usage);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->elementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, ELEMENTS_PER_CUBE * sizeof(unsigned int) * voxelTree->voxelCount, model->elementArray, usage);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            */
        }
        break;
    case MODEL_TYPE_GRID:
        {
            Grid grid;            
            memcpy(&grid, positionData, sizeof(Grid));
            model->vertexCount = (grid.vCount + grid.hCount) * 2 + 4;
            int totalVertexSize = model->vertexCount * vertex_size(vertexFormat) * sizeof(float);
            model->vertexArray = (float*) realloc(model->vertexArray,
                totalVertexSize
            );

            model_generate_grid_vertex_array(model->vertexArray, grid, colorData, vertexFormat);
            glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, totalVertexSize, model->vertexArray, usage);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        }
        break;
    case MODEL_TYPE_TEST_TRI:
        
        {
        model->vertexCount = 3;
        model->vertexArray = realloc(model->vertexArray,
            vertex_size(VERTEX_POSITION + VERTEX_RGBA) * sizeof(float) * model->vertexCount
        );
        int vertexArrayPointer = 0;
        vec3 p0 = {0, 0, 0};
        vec3 p1 = {1, 5, 0};
        vec3 p2 = {1, 0, 0};
        
        memcpy(&model->vertexArray[vertexArrayPointer],p0, sizeof(vec3));
        vertexArrayPointer += VERTEX_POSITION_SIZE;
        memcpy(&model->vertexArray[vertexArrayPointer], colorData, sizeof(vec4));
        vertexArrayPointer += VERTEX_RGBA_SIZE;

        
        memcpy(&model->vertexArray[vertexArrayPointer],p1, sizeof(vec3));
        vertexArrayPointer += VERTEX_POSITION_SIZE;
        memcpy(&model->vertexArray[vertexArrayPointer], colorData, sizeof(vec4));
        vertexArrayPointer += VERTEX_RGBA_SIZE;

        memcpy(&model->vertexArray[vertexArrayPointer],p2, sizeof(vec3));
        vertexArrayPointer += VERTEX_POSITION_SIZE;
        memcpy(&model->vertexArray[vertexArrayPointer], colorData, sizeof(vec4));
        vertexArrayPointer += VERTEX_RGBA_SIZE;

        glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 
            vertex_size(VERTEX_POSITION + VERTEX_RGBA) * sizeof(float) * 3, 
            model->vertexArray, 
            usage
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        break;
    case MODEL_TYPE_AABB:
        {
            Aabb aabb;
            memcpy(&aabb, positionData, sizeof(Aabb));
            int totalVertexSize = vertex_size(vertexFormat) * VERTICIES_PER_CUBE * sizeof(float);

            model->vertexArray = realloc(model->vertexArray, totalVertexSize);
            model->elementArray = realloc(model->elementArray, sizeof(unsigned int) * ELEMENTS_PER_CUBE);
            
            model_generate_aabb_vertex_array(model->vertexArray, aabb, colorData, vertexFormat);
            model_generate_aabb_element_array(model->elementArray, 0);
            
            glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, totalVertexSize, model->vertexArray, usage);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->elementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, ELEMENTS_PER_CUBE * sizeof(unsigned int), model->elementArray, usage);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        }
        break;
    }
}

void model_draw(Model *model, int vertexFormat, int method, int vertexCount){
    int vertexSize = vertex_size(vertexFormat);
    int shift = 0;
    glBindBuffer(GL_ARRAY_BUFFER, model->vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->elementBuffer);
    if((vertexFormat & VERTEX_POSITION) == VERTEX_POSITION){
        glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE, vertexSize * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        shift += 3;  
    }

    if((vertexFormat & VERTEX_RGBA) == VERTEX_RGBA){
        glVertexAttribPointer(1, 4,GL_FLOAT,GL_FALSE, vertexSize * sizeof(float), (void*)(sizeof(float) * shift));
        glEnableVertexAttribArray(1);
        shift += 4;
    }else if((vertexFormat & VERTEX_RGB) == VERTEX_RGB){
        glVertexAttribPointer(1, 3,GL_FLOAT,GL_FALSE, vertexSize * sizeof(float), (void*)(sizeof(float) * shift));
        glEnableVertexAttribArray(1);
        shift += 3;
    }

    if((vertexFormat & VERTEX_NORMAL) == VERTEX_NORMAL){
        glVertexAttribPointer(2, 3,GL_FLOAT,GL_FALSE, vertexSize * sizeof(float), (void*)(sizeof(float) * shift));
        glEnableVertexAttribArray(2);
        shift += 3;
    }
    switch (method)
    {
    case MODEL_DRAW_ELEMENTS:    
        glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
        break;
    
    case MODEL_DRAW_VERTICIES_LINES:
        glDrawArrays(GL_LINES, 0, vertexCount);
        break;
    case MODEL_DRAW_VERTICIES:
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

}