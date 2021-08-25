#ifndef FRAME_H
#define FRAME_H
#include <GL/glew.h>
#include "../include/frame_buffer_object.h"
typedef struct 
{
    GLuint buffer, colorTexture, depthStencilTexture;
}FrameBufferObject;

void frame_buffer_object_init(FrameBufferObject *frameBufferObject);
void frame_buffer_object_update(FrameBufferObject *frame, int width, int height);
#endif