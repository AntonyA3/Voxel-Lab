#include "../include/camera.h"

void camera_get_view_matrix(Camera camera, mat4x4 viewMatrix){
    quat yawQuat, pitchQuat, rotationQuat;
    mat4x4 rotationMatrix, distanceMatrix, offsetMatrix;
    vec3 up = {0,1,0};
    vec3 right = {1,0,0};
    mat4x4_translate(offsetMatrix, -camera.offset[0], -camera.offset[1], -camera.offset[2]);
    quat_rotate(yawQuat, -camera.yaw, up);
    quat_rotate(pitchQuat, -camera.pitch, right);
    quat_mul(rotationQuat, pitchQuat, yawQuat);
    mat4x4_from_quat(rotationMatrix, rotationQuat);
    mat4x4_translate(distanceMatrix, 0, 0, -camera.distance);

    mat4x4_mul(viewMatrix, rotationMatrix,offsetMatrix);
    mat4x4_mul(viewMatrix, distanceMatrix, viewMatrix );
}

void camera_get_projection_matrix(Camera camera, mat4x4 projectionMatrix){
    switch (camera.cameraMode)
    {
    case CAMERA_MODE_PERSPECTIVE:
        mat4x4_perspective(projectionMatrix, camera.fov, camera.aspectRatio, camera.near, camera.far);
        break;
    case CAMERA_MODE_ORTHOGRAPHIC:    
        mat4x4_ortho(projectionMatrix,
            -camera.dWidth, camera.dWidth, 
            -camera.dHeight, camera.dHeight,
            camera.near, camera.far
        );
        break;
    }
}

void camera_get_position(Camera camera, vec3 position){
    vec4 identityVector = {0, 0, 0, 1}, distanceVector, rotatedVector, offsetVector;
    quat yawQuat, pitchQuat, rotationQuat;
    mat4x4 distanceMatrix, rotationMatrix, offsetMatrix;

    vec3 up = {0, 1, 0};
    vec3 right = {1, 0, 0};
    quat_rotate(yawQuat, camera.yaw, up);
    quat_rotate(pitchQuat, camera.pitch, right);
    quat_mul(rotationQuat, yawQuat,pitchQuat );
    mat4x4_from_quat(rotationMatrix, rotationQuat);
    mat4x4_translate(distanceMatrix, 0, 0, camera.distance);
    mat4x4_translate(offsetMatrix, camera.offset[0], camera.offset[1], camera.offset[2]);

    mat4x4_mul_vec4(distanceVector, distanceMatrix, identityVector);

    mat4x4_mul_vec4(rotatedVector, rotationMatrix, distanceVector);
    mat4x4_mul_vec4(offsetVector, offsetMatrix, rotatedVector);

    position[0] = offsetVector[0];
    position[1] = offsetVector[1];
    position[2] = offsetVector[2];
}


