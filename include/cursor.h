#ifndef CURSOR_H
#define CURSOR_H
#include "../include/button.h"
#include "../include/ray.h"
typedef struct
{
    float prevX, prevY;
    float x, y;
    float dx, dy;
    float clipX, clipY;
    union 
    {
        struct{
            Button primaryButton, secondaryButton; 
        };
        Button mouseButtons[2]; 
    };
    Ray ray;
    int isHit;
    int hitSide;
    int hitEntity;
    float hitDistance;
    vec3 hit;
}Cursor;


#endif