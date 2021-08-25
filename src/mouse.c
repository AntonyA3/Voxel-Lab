#include "../include/mouse.h"

void mouse_init(Mouse *mouse, float x, float y){
    mouse->x = x;
    mouse->y = y;
    mouse->prevX = x;
    mouse->prevY= y;
    mouse->dx = 0;
    mouse->dy = 0;
}
void mouse_update(Mouse *mouse, float x, float y){
    mouse->dx = x - mouse->prevX;
    mouse->dy = y - mouse->prevY;
    mouse->prevX = mouse->x;
    mouse->prevY = mouse->y;
    mouse->x = x;
    mouse->y = y;
    
}

