

#ifndef BOX_GENERATOR_H
#define BOX_GENERATOR_H
#include "../include/linmath.h"
#include "../include/pos_color32_vertex.h"
#include "../include/color_rgba_f.h"

/*
void box_generate_position(vec3 origin, vec3 halfExtents, float *vertexArray, unsigned int *elementsArray);
*/
void box_generate_box_pos_color_32_vertex(vec3 origin,ColorRgbaF color, vec3 halfExtents,PosColor32Vertex *vertexArray, unsigned int *elementArray);

#endif
