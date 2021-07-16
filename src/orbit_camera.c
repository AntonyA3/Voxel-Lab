#include "../include/orbit_camera.h"
    
void init_orbit_camera(OrbitCamera* camera,vec3 offset, float yaw, 
float pitch, float fov, float near, float far, float ar, 
    float distance){
        memcpy(camera->offset,offset, sizeof(vec3));
        camera->yaw = yaw;
        camera->pitch = pitch;
        camera->fov = fov;
        camera->near = near;
        camera->far = far;
        camera->aspectRat = ar;
        camera->distance = distance;
        camera->state = ORBIT_CAMERA_STATE_NOT_ORBITING;
}


void get_view_matrix_orbit_camera(OrbitCamera camera, mat4x4 viewMat){
    mat4x4 distMat, transMat, rotMat;
    quat camYaw, camPitch, camRot;
    vec3 up = {0,1,0};
    vec3 right = {1,0,0}; 
    mat4x4_translate(transMat, -camera.offset[0], -camera.offset[1], -camera.offset[2]);
    quat_rotate(camYaw, -camera.yaw, up);
    quat_rotate(camPitch, -camera.pitch, right);
    quat_mul(camRot,camPitch,camYaw);
    mat4x4_from_quat(rotMat, camRot);
    mat4x4_translate(distMat,0.0,0.0, -camera.distance);

    mat4x4_mul(viewMat, rotMat, transMat);
    mat4x4_mul(viewMat, distMat, viewMat);
        
}
void get_projection_matrix_orbit_camera(OrbitCamera camera, mat4x4 projMat){
    mat4x4_perspective(projMat, camera.fov, camera.aspectRat, camera.near, camera.far);

}

void get_position_orbit_camera(OrbitCamera camera, vec3 position){
    mat4x4 rotMat, distMat, transMat;
    quat camYaw, camPitch, camRot;
    Vector3 up = {{0,1,0}};
    Vector3 right = {{1,0,0}}; 
    Vector4 pos = {{0,0,0,1}};
    vec4 tempPos;

    quat_rotate(camYaw, camera.yaw, up.vec);
    quat_rotate(camPitch, camera.pitch, right.vec);
    quat_mul(camRot,camYaw,camPitch);
    mat4x4_from_quat(rotMat, camRot);
    mat4x4_translate(distMat, 0, 0, camera.distance);
    memcpy(tempPos, pos.vec, sizeof(vec4));   
    mat4x4_mul_vec4(pos.vec, distMat, tempPos);
    memcpy(tempPos, pos.vec, sizeof(vec4));
    mat4x4_mul_vec4(pos.vec, rotMat, tempPos);    
    memcpy(position, pos.xyz,sizeof(vec3));
}
