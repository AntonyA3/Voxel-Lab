#ifndef CAMERA_H
#define CAMERA_H
#include "../linmath/linmath.h"
enum camera_projection{CAMERA_ORTHOGRAPHIC, CAMERA_PERSPECTIVE};



typedef struct {
    int isRotating;
}CameraController;
typedef struct
{
    int projection;
    float yaw, pitch, distance;
    float near, far, fov;
    vec3 offset;
}Camera;

void camera_update_yaw(Camera *camera, float dx, float dt);
void camera_update_pitch(Camera *camera, float dy, float dt);

void camera_init(Camera *camera, float yaw, float pitch, 
    float distance, float near, float far, float fov);
void camera_update_yaw(Camera *camera, float dx, float dt);
void camera_update_pitch(Camera *camera, float dy, float dt);
void camera_get_position(Camera camera, vec3 position);
void camera_update_view_matrix(Camera *camera);
void camera_get_view_matrix(Camera camera, mat4x4 viewMatrix);
void camera_update_position(Camera *camera);
#endif