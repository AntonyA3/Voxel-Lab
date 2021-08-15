#ifndef SCENE_H
#define SCENE_H
#include "../linmath/linmath.h"
typedef struct
{
    float lightYaw, lightPitch;
    vec3 lightDirection;
}Scene;
#endif