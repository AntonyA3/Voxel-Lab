#ifndef MOUSE_H
#define MOUSE_H
#include "../include/button.h"
typedef struct
{
    float prevX, prevY;
    float x, y;
    float dx, dy;
    union 
    {
        struct{
            Button primaryButton, secondaryButton; 
        };
        Button buttons[2]; 
    };

    
}Mouse;
void mouse_init(Mouse *mouse, float x, float y);
void mouse_update(Mouse *mouse, float x, float y);
#endif