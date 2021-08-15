#include "../include/frame.h"

void frame_update(Frame *frame, int width, int height){
    glBindFramebuffer(GL_FRAMEBUFFER, frame->buffer);
    glBindTexture(GL_TEXTURE_2D, frame->colorTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
        width, 
        height, 
        0, GL_RGB, GL_UNSIGNED_BYTE, NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame->colorTexture, 0);
    
    glBindTexture(GL_TEXTURE_2D, frame->depthStencilTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,  
        width, 
        height, 
        0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
    );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, frame->depthStencilTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
