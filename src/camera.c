#include "../include/camera.h"


void camera_init(Camera *camera, float yaw, float pitch, 
    float distance, float near, float far, float fov){
        camera->yaw = yaw;
        camera->pitch = pitch;
        camera->distance = distance;
        camera->near = near;
        camera->far = far;
        camera->fov = fov;
}

void camera_get_view_matrix(Camera camera, mat4x4 viewMatrix){
    
    quat yawQuat, pitchQuat, rollQuat, rotationQuat, rotationQuat2;
    mat4x4 rotationMatrix, distanceMatrix, offsetMatrix;
    vec3 right = {1, 0, 0};
    vec3 up = {0, 1, 0};
    vec3 forward = {0, 0, 1};

    mat4x4_identity(viewMatrix);
    mat4x4_translate(offsetMatrix, 
        -camera.offset[0],
        -camera.offset[1], 
        -camera.offset[2]
    );
    
    quat_rotate(yawQuat, -camera.yaw, up);
    quat_rotate(pitchQuat, -camera.pitch, right);
    quat_rotate(rollQuat, M_PI, forward);
    quat_mul(rotationQuat, yawQuat, rollQuat);
    quat_mul(rotationQuat2,pitchQuat, rotationQuat);
    mat4x4_from_quat(rotationMatrix, rotationQuat2);
    
    mat4x4_translate(distanceMatrix, 0, 0, -camera.distance);

    mat4x4_mul(viewMatrix, rotationMatrix, offsetMatrix);
    mat4x4_mul(viewMatrix, distanceMatrix, viewMatrix);
    
}


void camera_update_view_matrix(Camera *camera) {
    /*
    vec3 right = {1, 0, 0};
    vec3 up = {0, 1, 0};
    vec3 forward = {0, 0, 1};
    quat yawQuat, pitchQuat, rollQuat, rotationQuat, rotationQuat2;
    mat4x4 rotationMatrix, distanceMatrix, offsetMatrix;
    
    mat4x4_translate(offsetMatrix, 
        -camera->offset[0],
        -camera->offset[1], 
        -camera->offset[2]
    );
    
    quat_rotate(yawQuat, -camera->yaw, up);
    quat_rotate(pitchQuat, -camera->pitch, right);
    quat_rotate(rollQuat, M_PI, forward);
    quat_mul(rotationQuat, yawQuat, rollQuat);
    quat_mul(rotationQuat2,pitchQuat, rotationQuat);
    mat4x4_from_quat(rotationMatrix, rotationQuat2);
    
    mat4x4_translate(distanceMatrix, 0, 0, -camera->distance);

    mat4x4_mul(camera->viewMat, rotationMatrix,offsetMatrix);
    mat4x4_mul(camera->viewMat, distanceMatrix, camera->viewMat);
    */    
}

void camera_get_position(Camera camera, vec3 position){
    quat yawQuat, pitchQuat, rollQuat, rotationQuat, rotationQuat2;
    mat4x4 distanceMatrix, rotationMatrix, offsetMatrix;

    vec4 identityVector = {0, 0, 0, 1};
    vec4 distanceVector, rotatedVector, offsetVector;
    vec3 right = {1, 0, 0};
    vec3 up = {0, 1, 0};
    vec3 forward = {0, 0, 1};
    quat_rotate(yawQuat, camera.yaw, up);
    quat_rotate(pitchQuat, camera.pitch, right);
    quat_rotate(rollQuat, M_PI, forward);

    quat_mul(rotationQuat, yawQuat, pitchQuat );
    quat_mul(rotationQuat2, rollQuat, rotationQuat);
    mat4x4_from_quat(rotationMatrix, rotationQuat2);
    mat4x4_translate(distanceMatrix, 0, 0, camera.distance);
    mat4x4_translate(offsetMatrix, camera.offset[0], camera.offset[1], camera.offset[2]);

    mat4x4_mul_vec4(distanceVector, distanceMatrix, identityVector);
    mat4x4_mul_vec4(rotatedVector, rotationMatrix, distanceVector);
    mat4x4_mul_vec4(offsetVector, offsetMatrix, rotatedVector);
    memcpy(position, offsetVector, sizeof(vec3));
    
}      