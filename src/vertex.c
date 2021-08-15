#include "../include/vertex.h"

int vertex_size(int format){
    int size = 0;
    size += ((format & VERTEX_POSITION) == VERTEX_POSITION) * 3;
    if((format & VERTEX_RGBA) == VERTEX_RGBA){
        size += 4;
    }else if((format & VERTEX_RGB) == VERTEX_RGB){
        size += 3;
    }
    size += ((format & VERTEX_NORMAL) == VERTEX_NORMAL) * 3;
    return size;
}