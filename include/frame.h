#ifndef FRAME_H
#define FRAME_H
#include <GL/glew.h>
#include "../include/frame.h"
typedef struct 
{
    GLuint buffer, colorTexture, depthStencilTexture;
}Frame;


void frame_update(Frame *frame, int width, int height);
#endif