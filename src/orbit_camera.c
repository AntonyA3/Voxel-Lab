
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
    mat4x4 viewMat;
    vec4 forward; 
    get_view_matrix_orbit_camera(camera,viewMat);
    mat4x4_row(forward, viewMat, 2);
    vec3 forward3 = {forward[0], forward[1], forward[2]};
    vec3_scale(position, forward3, camera.distance);
    vec3_add(position,position, camera.offset);    
    //position[1] = -position[1];
}

/*
void getProjMat(OrbitCamera* camera, mat4x4* projMat){
    mat4x4_perspective(projMat, camera->fov, camera->aspectRat, camera->near, camera->far);

}


void getViewMat(OrbitCamera* camera, mat4x4* viewMat){
        mat4x4 distMat, transMat, rotMat;
        quat camYaw, camPitch, camRot;
        vec3 up = {0,1,0};
        vec3 right = {1,0,0}; 
        mat4x4_translate(transMat, -camera->offset[0], -camera->offset[1], -camera->offset[2]);
        quat_rotate(camYaw, -camera->yaw, up);
        quat_rotate(camPitch, -camera->pitch, right);
        quat_mul(camRot,camPitch,camYaw);
        mat4x4_from_quat(rotMat, camRot);
        mat4x4_translate(distMat,0.0,0.0, -camera->distance);

        mat4x4_mul(viewMat, rotMat, transMat);
        mat4x4_mul(viewMat, distMat, viewMat);
        

}



void panCamera(OrbitCamera* camera, float horizontal, float vertical){
    vec4 rV;
    vec4 uV;
    mat4x4 viewMat;

    getViewMat(camera,&viewMat);

    mat4x4_row(rV,viewMat,0);
    mat4x4_row(uV,viewMat,1);
    vec3 rightV = {rV[0], rV[1], rV[2]};
    vec3 upV = {uV[0], uV[1], uV[2]};
    vec3_scale(rightV, rightV, horizontal*0.016);
    vec3_scale(upV, upV, vertical*0.016);

    vec3_add(camera->offset,camera->offset, rightV);
    vec3_add(camera->offset, camera->offset, upV);
}


void orbitCameraDoOrbit(OrbitCamera* orbitCam, float dx, float dy){
    switch (orbitCam->state)
    {
        case orbit_cam_state_not_orbit:
            break;
        case orbit_cam_state_is_orbit:
            orbitCam->yaw += dx* 0.016;
            orbitCam->pitch += dy * 0.016;
            break;
    }

}

*/