#ifndef VERTEX_H
#define VERTEX_H
#include <stdlib.h>

#include <math.h>
#define VERTEX_POSITION_SIZE 3
#define VERTEX_NORMAL_SIZE 3
#define VERTEX_RGB_SIZE 3
#define VERTEX_RGBA_SIZE 4

enum vertex_flag{VERTEX_POSITION = 1, VERTEX_NORMAL = 2, VERTEX_RGB = 4 ,VERTEX_RGBA = 8};
int vertex_size(int format);
#endif