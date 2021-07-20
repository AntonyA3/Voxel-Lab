#ifndef APP_H
#define APP_H
#include <GL/glew.h>
#include "../include/color.h"
#include "../include/cursor.h"
#include "../include/grid.h"
#include "../include/orbit_camera.h"
#include "../include/menu_bar.h"
#include "../include/vector2.h"
#include "../include/vector3.h"
#include "../include/vector4.h"
#include "../include/app_viewport.h"
#include "../include/aabb.h"
#include "../include/ray.h"
#include "../include/prim_fact.h"
#include "../include/voxel.h"

enum app_mode{APP_MODE_DEBUG, APP_MODE_DEFAULT};
enum shader_id{SHADER_ID_GRID_SHADER, SHADER_ID_VOXEL_SHADER, SHADER_ID_PRIM_SHADER, SHADER_ID_SHADER_COUNT};
enum color_data{COLOR_DATA_BACKGROUND_COLOR, COLOR_DATA_COUNT};
enum voxel_edit{
    VOXEL_EDIT_ADD_VOXEL = 0b10, 
    VOXEL_EDIT_DELETE_VOXEL = 0b01
};

enum do_update {
    DO_UPDATE_VIEW_MAT = 0b100, 
    DO_UPDATE_PROJ_MAT = 0b010, 
    DO_UPDATE_INV_MAT = 0b001, 
    DO_UPDATE_NORMALIZED_CURSOR = 0b0001,
    DO_UPDATE_CURSOR_RAY = 0b00001,
    DO_RAYCAST_VOXEL_MODEL = 0b000001,
    DO_RAYCAST_GRID = 0b000001,
};

typedef struct App{
    int appMode; 
    Cursor cursor;
    OrbitCamera orbitCamera;
    Vector3 orbitCameraPosition;
    mat4x4 projMat, viewMat, inverseMat;
    int leftBtn, rightBtn;
    int cursorButtonState[CURSOR_BUTTON_COUNT];
    int appCursorFocus;
    int entityHit;
    int cameraInsideVoxel;
    int updateRequest;
    int voxelManipulationMode, voxelManipulationShape;
    Sphere sphereBrush;
    Aabb boxBrush;
    ColorRGBAf sceneBackColor;
    AppViewport renderViewport, windowViewport;
    float menuBarHeight;
    float menuBarButtonWidth, propertySelectorWidth, propertyWidth, renderPanelWidth;
    vec2 fileMenuSize;
    Grid floorGrid;
    GridVertex* floorGridModel;
    int floorGridVertexCount;
    ColorRGBAf colorData[COLOR_DATA_COUNT];
    GLuint shaderId[SHADER_ID_SHADER_COUNT];
    Voxel* voxelHead;
    GLuint floorGridVbo, frameTexture, frameDepthTexture, frameBuffer,
    voxelModelVertexBuffer, vertexModelIndexBuffer;
}App;
#endif