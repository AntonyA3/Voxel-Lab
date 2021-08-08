#include "../include/box_generator.h"



void box_generate_box_pos_color_32_vertex(vec3 origin,ColorRgbaF color, vec3 halfExtents,PosColor32Vertex *vertexArray, unsigned int *elementArray){
     PosColor32Vertex verticies[8] = {
        {{-1.0, -1.0, -1.0, 1.0,0.0,1.0,1.0}},
        {{-1.0, 1.0, -1.0, 1.0,1.0,1.0,1.0}},
        {{1.0, 1.0, -1.0, 1.0,0.0,1.0,1.0}},
        {{1.0, -1.0, -1.0, 1.0,1.0,1.0,1.0}},
        {{-1.0, -1.0, 1.0, 1.0,0.0,1.0,1.0}},
        {{-1.0, 1.0, 1.0, 1.0,1.0,1.0,1.0}},
        {{1.0, 1.0, 1.0, 1.0,0.0,1.0,1.0}},
        {{1.0, -1.0, 1.0, 1.0,0.0,1.0,1.0}},
    };
    
    for(int i = 0; i < 8; i++){
    
        verticies[i].x *= halfExtents[0];
        verticies[i].y *= halfExtents[1];
        verticies[i].z *= halfExtents[2];
        vec3_add(verticies[i].position, verticies[i].position, origin);
        memcpy(verticies[i].color, color.color, sizeof(float) * 4);
        
    }
    unsigned int elements[36] = {
        0, 1, 2, 0, 2, 3,
        5, 4, 1, 4, 1, 0,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7,
        3, 2, 6, 3, 6, 7,
        7, 6, 5, 7, 5, 4
    };
    memcpy(vertexArray, verticies, sizeof(PosColor32Vertex) * 8);
    memcpy(elementArray, elements, sizeof(unsigned int) * 36);

}

