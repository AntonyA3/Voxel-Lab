#ifndef ORBIT_CAMERA_H
#define ORBIT_CAMERA_H

#include "linmath.h"
#include "vector3.h"
#include "vector4.h"
enum orbit_cam_state{ORBIT_CAMERA_STATE_NOT_ORBITING, ORBIT_CAMERA_STATE_IS_ORBITING};
typedef struct OrbitCamera
{
    vec3 offset;
    float yaw, pitch;
    float fov;
    float near;
    float far;
    float aspectRat;
    float distance;
    int state;
}OrbitCamera;

void init_orbit_camera(OrbitCamera *camera,vec3 offset, float yaw, 
float pitch, float fov, float near, float far, float ar, 
    float distance);

void get_view_matrix_orbit_camera(OrbitCamera camera, mat4x4 viewMat);
void get_projection_matrix_orbit_camera(OrbitCamera camera, mat4x4 projMat);
void get_position_orbit_camera(OrbitCamera camera, vec3 position);
/*
void do_orbit_of_orbit_camera(OrbitCamera *camera, float dx, float dy, int trigger);
void do_pan_orbit_camera(OrbitCamera* camera, float horizontal, float vertical);

void do_get_orbit_camera_rotation_matrix(OrbitCamera* camera, mat4x4* rotMat);
void cameraOrbitEvent(OrbitCamera* camera, int buttonState, float dx, float dy);
void orbitCameraPosition(OrbitCamera camera, vec3 position);
void orbitCameraDoOrbit(OrbitCamera* orbitCam, float dx, float dy);
*/
#endif