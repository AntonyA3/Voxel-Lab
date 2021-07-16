#include "../include/prim_fact.h"

void generate_cube_from_centre(vec3 center, vec3 halfExtents,  Vertex* vertexArray, unsigned int *elementArray){
    Vertex model[8] = {
        {-1.0, -1.0, -1.0},
        {-1.0, 1.0, -1.0},
        {1.0, 1.0, -1.0},
        {1.0, -1.0, -1.0},
        {-1.0, -1.0, 1.0},
        {-1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0},
        {1.0, -1.0, 1.0}
    };
    for(int i = 0; i < 8; i++){
        model[i].x = model[i].x * halfExtents[0] + center[0];
        model[i].y = model[i].y * halfExtents[1] + center[1];
        model[i].z = model[i].z * halfExtents[2] + center[2];
    }
    unsigned int elements[36] = {
        4,5,1, 4,1,0,
        1,5,6, 1,6,2,
        0,1,2, 0,2,3,
        4,0,3, 4,3,7,
        3,2,6, 3,6,7,
        7,6,5, 7,5,4
    };
    memcpy(vertexArray, model, 8 * sizeof(Vertex));
    memcpy(elementArray, elements, 36 * sizeof(unsigned int));
}

void generate_cube_from_line(vec3 start, vec3 end, float thickness, Vertex* vertexArray, unsigned int *elementArray){
    vec3 xVec, yVec, zVec, up = {0,1,0};
    
    vec3_sub(zVec, end, start);
    vec3_norm(zVec, zVec);
    vec3_mul_cross(xVec, up, zVec);
    vec3_mul_cross(yVec, zVec,xVec );
    
    vec3_scale(xVec, xVec, thickness);
    vec3_scale(yVec, yVec, thickness);

    vec3 v0, v1, v2, v3, v4, v5, v6, v7;
    
    vec3_sub(v0, start, xVec);
    vec3_sub(v0, v0, yVec);

    vec3_add(v1, v0, yVec);
    vec3_add(v1, v1, yVec);

    vec3_add(v2, v1, xVec);
    vec3_add(v2, v2, xVec);

    vec3_sub(v3, v2, yVec);
    vec3_sub(v3, v3, yVec);

    vec3_sub(zVec, end, start);
    vec3_add(v4, v0, zVec);
    vec3_add(v5, v1, zVec);
    vec3_add(v6, v2, zVec);
    vec3_add(v7, v3, zVec);

    Vertex model[8] = {
        {v0[0], v0[1], v0[2]},
        {v1[0], v1[1], v1[2]},
        {v2[0], v2[1], v2[2]},
        {v3[0], v3[1], v3[2]},
        {v4[0], v4[1], v4[2]},
        {v5[0], v5[1], v5[2]},
        {v6[0], v6[1], v6[2]},
        {v7[0], v7[1], v7[2]}
    };
    unsigned int elements[36] = {
        4,5,1, 4,1,0,
        1,5,6, 1,6,2,
        0,1,2, 0,2,3,
        4,0,3, 4,3,7,
        3,2,6, 3,6,7,
        7,6,5, 7,5,4
    };
    
    memcpy(vertexArray, model, 8 * sizeof(Vertex));
    memcpy(elementArray, elements, 36 * sizeof(unsigned int));
}