#ifndef APP_H
#define APP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../include/camera.h"
#include "../include/cursor.h"
#include "../include/cursor_button_id.h"
#include "../include/color_rgba_f.h"
#include "../include/ray.h"
#include "../include/grid_xz.h"
#include "../include/pos_color32_vertex.h"
#include "../include/vertex_buffer_id.h"
#include "../include/element_buffer_id.h"
#include "../include/shader_program_id.h"
#include "../include/ray_hit_entity.h"
#include "../include/cursor_button_state.h"
#include "../include/voxel_model.h"
#include "../include/box_generator.h"
#include "../include/aabb.h"
#include "../include/aabb_side.h"
#include "../include/frame_buffer_id.h"
#include "../include/texture_id.h"
typedef struct{
    Cursor cursor;
    Camera camera;
    GridXZ floorGrid;
    PosColor32Vertex *floorGridVertexArray;
    int floorGridVertexCount;

    vec3 cameraPosition;
    GLuint vertexBufferId[VERTEX_BUFFER_ID_COUNT];
    GLuint elementBufferId[ELEMENT_BUFFER_ID_COUNT];
    GLuint frameBufferId[FRAME_BUFFER_ID_COUNT];
    GLuint textureId[TEXTURE_ID_COUNT];
    GLuint shaderProgramId[SHADER_PROGRAM_ID_COUNT];

    mat4x4 projectionMatrix;
    mat4x4 viewMatrix;
    mat4x4 projectionViewMatrix;
    mat4x4 invertedMatrix;
    ColorRgbaF backgroundColor;
    float menuBarHeight;
    float windowWidth, windowHeight, windowRatio;
    float appViewWidth, appViewHeight, appViewRatio;
    float propertyWidth, propertyHeight;
    float propertySelectorWidth, propertySelectorHeight;
    int cursorButtonDown[CURSOR_BUTTON_ID_COUNT];
    int cursorButtonState[CURSOR_BUTTON_ID_COUNT];
    Ray cursorRay;
    int cursorRayDidHit;
    int cursorHitEntity;
    int cursorSideHit;
    vec3 cursorRayHitPoint;

    VoxelModel voxelModel;
    PosColor32Vertex *voxelModelVertexArray;
    unsigned int *voxelModelElementArray;
    GLuint voxelModelVertexBuffer, voxelModelElementBuffer;
    GLuint voxelHeadVertexBuffer, voxelHeadElementBuffer;
    int voxelCount;
}App;

void app_update_window_glfw(App *app, GLFWwindow *window);
void app_update_cursor_glfw(App *app, GLFWwindow *window);
void app_update_cursor_ray(App *app);
void app_generate_floor_grid_vertex_array(App *app);
void app_use_pos_color_32_vertex();
void app_ray_vs_y0(App *app, int *didHit, Ray ray, int *entity, float *bestDistance);
void app_ray_vs_aabb(App *app, Aabb aabb, int *didHit, Ray ray, float *bestDistance, int *side);
void app_ray_vs_voxel_model(App *app, int *didHit, VoxelModel voxelModel, Ray ray, int *entity, float *bestDistance, int *side);
void app_ray_vs_voxel(App *app, int *didHit, Voxel *voxel, Ray ray, int *entity, float *bestDistance, int *side);
void app_update_ray_hit_model(App *app);
void app_update_voxel_model(App *app);
void app_update_voxel_head_model(App* app);
void app_point_at_ray_distance(Ray ray, float distance, vec3 point);
void app_hit_target_from_voxel_model_hit(App *app, int side, float *targetX, float *targetY, float *targetZ);

int app_point_in_yz_plane(vec3 point,float z, float depth, float y, float height);
int app_point_in_xy_plane(vec3 point,float x, float width, float y, float height);
int app_point_in_xz_plane(vec3 point,float x, float width, float z, float depth);

int app_get_cursor_button_next_state(int state, int isDown);
int app_generate_shader_program_from_file(App *app, const char *vertexShaderPath, const char *fragmentShaderPath);

#endif