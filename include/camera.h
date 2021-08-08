#ifndef CAMERA_H
#define CAMERA_H
#include "../include/linmath.h"
#include "../include/camera_mode.h"

typedef struct Camera{
    int cameraMode;
    float dWidth, dHeight;
    float yaw, pitch, distance,
    fov, near, far, aspectRatio;
    vec3 offset;
}Camera;

void camera_get_view_matrix(Camera camera, mat4x4 viewMatrix);
void camera_get_projection_matrix(Camera camera, mat4x4 projectionMatrix);
void camera_get_position(Camera camera, vec3 position);
#endif