#ifndef CAMERA_H
#define CAMERA_H
#include "../linmath/linmath.h"

enum camera_projection{CAMERA_ORTHOGRAPHIC, CAMERA_PERSPECTIVE};

typedef struct
{
    int projection;
    float yaw, pitch, distance;
    float aspectRatio, near, far, fov;
    vec3 offset, position;
    quat yawQuat, pitchQuat, rollQuat;
    mat4x4 viewMat, projMat, inverseMat;
}Camera;

void camera_init(Camera *camera, float yaw, float pitch, 
    float distance, float near, float far, float fov);
#endif