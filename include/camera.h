#include "linmath.h"
#include <stdlib.h>

enum orbit_cam_state{orbit_cam_state_not_orbit, orbit_cam_state_is_orbit};
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

void initOrbitCamera(OrbitCamera *camera,vec3 offset, float yaw, 
float pitch, float fov, float near, float far, float ar, 
    float distance);

void getViewMat(OrbitCamera *camera, mat4x4 *viewMat);
void getProjMat(OrbitCamera *camera, mat4x4 *projMat);
void orbCamera(OrbitCamera *camera, float dx, float dy, int trigger);
void panCamera(OrbitCamera* camera, float horizontal, float vertical);

void getCameraObjectRotMat(OrbitCamera* camera, mat4x4* rotMat);
void cameraOrbitEvent(OrbitCamera* camera, int buttonState, float dx, float dy);
void orbitCameraPosition(OrbitCamera camera, vec3 position);
void orbitCameraDoOrbit(OrbitCamera* orbitCam, float dx, float dy);
