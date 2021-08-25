#include "../include/floor_grid.h"

int floor_grid_vertex_count(FloorGrid floorGrid){
    return (floorGrid.grid.vCount + floorGrid.grid.hCount) * 2 + 4;
}

void floor_grid_generate_vertex_array(FloorGrid *floorGrid, float *vertexArray){
    Grid *grid = &floorGrid->grid;
    vec3 horLength;
    vec3 vertLength; 
    vec3 gridPoint;   
    vec3_scale(vertLength, grid->vectorV, grid->vCount);
    vec3_scale(horLength, grid->vectorH, grid->hCount);
    vec4 white = {1.0,1.0,1.0,1.0};
    memcpy(gridPoint, grid->min, sizeof(vec3));
    for(int h = 0; h <= grid->hCount; h++){
        memcpy(vertexArray, gridPoint,sizeof(vec3));
        vertexArray += 3;
        memcpy(vertexArray, white, sizeof(vec4));
        vertexArray += 4;  
        vec3_add(gridPoint, gridPoint, vertLength);


        memcpy(vertexArray, gridPoint, sizeof(vec3));
        vertexArray += 3;
        
        memcpy(vertexArray, white, sizeof(vec4));
        vertexArray += 4;  
        vec3_sub(gridPoint, gridPoint, vertLength);
        vec3_add(gridPoint, gridPoint, grid->vectorH);
    }
    memcpy(gridPoint, grid->min, sizeof(vec3));
    for(int v = 0; v <= grid->vCount; v++){
        memcpy(vertexArray, gridPoint,sizeof(vec3));
        vertexArray += 3;
        
        memcpy(vertexArray, white, sizeof(vec4));
        vertexArray += 4;  
        vec3_add(gridPoint, gridPoint, horLength);
            
        memcpy(vertexArray, gridPoint, sizeof(vec3));
        vertexArray += 3;
        
        memcpy(vertexArray, white, sizeof(vec4));
        vertexArray += 4;  

        vec3_sub(gridPoint, gridPoint, horLength);
        vec3_add(gridPoint, gridPoint,grid->vectorV);
    }
    
}