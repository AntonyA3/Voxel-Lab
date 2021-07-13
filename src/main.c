#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "linmath.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#include "../nuklear/nuklear.h"
#include "../nuklear/nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define VERTEX_PER_CUBE 8
#define INDICIES_PER_CUBE 36 

#ifndef COLOR_H
#define COLOR_H
#include "../include/color.h"
#endif

#ifndef CURSOR_H
#define CURSOR_H
#include "../include/cursor.h"
#endif

#ifndef GRID_H
#define GRID_H
#include "../include/grid.h"
#endif

#ifndef ORBIT_CAMERA_H
#define ORBIT_CAMERA_H
#include "../include/orbit_camera.h"
#endif

#ifndef MENU_BAR_H
#define MENU_BAR_H
#include "../include/menu_bar.h"
#endif

#ifndef VECTOR2_H
#define VECTOR2_H
#include "../include/vector2.h"
#endif

#ifndef VECTOR3_H
#define VECTOR3_H
#include "../include/vector3.h"
#endif

#ifndef APP_VIEWPORT_H
#define APP_VIEWPORT_H
#include "../include/app_viewport.h"
#endif

#ifndef VOXEL_H
#define VOXEL_H
#include "../include/voxel.h"
#endif

#ifndef AABB_H
#define AABB_H
#include "../include/aabb.h"
#endif

#ifndef RAY_H
#define RAY_H
#include "../include/ray.h"
#endif
enum app_cursor_focus{APP_CURSOR_FOCUS_GUI,APP_CURSOR_FOCUS_RENDER_VIEWPORT, APP_CURSOR_FOCUS_OUTSIDE_WINDOW};
enum app_mode{APP_MODE_DEBUG, APP_MODE_DEFAULT};
enum shader_id{SHADER_ID_GRID_SHADER, SHADER_ID_VOXEL_SHADER, SHADER_ID_SHADER_COUNT};
enum ray_hit_entity{RAY_HIT_NOTHING, RAY_HIT_ENTITY_GRID, RAY_HIT_ENTITY_VOXEL, RAY_HIT_Y_PLANE};
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

typedef struct AppGlobals{
    float yScroll;
    float xScroll;
}AppGlobals;



typedef struct RaycastResult{
    int entityHitType;
    vec3 hitPoint, hitNormal;
}RaycastResult;



typedef struct App{
    Vector2 cursorPos;
    Vector2 cursorDelta;
    Vector2 normCursorPos;
    Ray cursorRay;
    int cursorRayHit;
    Vector3 cursorPos3D;
    OrbitCamera orbitCamera;
    vec3 orbitCameraPosition;
    mat4x4 projMat, viewMat, inverseMat;
    int leftBtn, rightBtn;
    int cursorButtonState[CURSOR_BUTTON_COUNT];
    int appCursorFocus;
    int cameraInsideVoxel;
    int updateRequest;
    int voxelManipulationMode;
    ColorRGBAf sceneBackColor;
    AppViewport renderViewport, windowViewport;
    float menuBarHeight;
    float menuBarButtonWidth, propertySelectorWidth, propertyWidth, renderPanelWidth;
    vec2 fileMenuSize;
    Grid floorGrid;
    GridVertex* floorGridModel;
    int floorGridVertexCount;
    ColorRGBAf colorData[COLOR_DATA_COUNT];
    GLuint shaderID[SHADER_ID_SHADER_COUNT];
   
    GLuint floorGridVbo, frameTexture, frameDepthTexture, frameBuffer,
    voxelModelVertexBuffer, vertexModelIndexBuffer;
    Voxel* voxelHead;
    VoxelVertex* voxelModelVerticies;
    unsigned int* voxelModelIndicies;
    int voxelCount;
}App;

void do_cursor_event(App* app){
    app->cursorButtonState[CURSOR_BUTTON_LEFT] = 
            cursor_button_next_state(app->cursorButtonState[CURSOR_BUTTON_LEFT], app->leftBtn == GLFW_PRESS
        );
    app->cursorButtonState[CURSOR_BUTTON_RIGHT] =
        cursor_button_next_state(app->cursorButtonState[CURSOR_BUTTON_RIGHT], app->rightBtn == GLFW_PRESS
    );
}

void do_orbit_camera_event(App* app){
    switch (app->orbitCamera.state)
        {
        case ORBIT_CAMERA_STATE_IS_ORBITING:
            if(app->cursorButtonState[CURSOR_BUTTON_RIGHT] == CURSOR_BUTTON_STATE_UP){
                app->orbitCamera.state = ORBIT_CAMERA_STATE_NOT_ORBITING;
            }
            app->orbitCamera.yaw += 0.016 * app->cursorDelta.x;
            app->orbitCamera.pitch += 0.016 * app->cursorDelta.y;
            break;
        case ORBIT_CAMERA_STATE_NOT_ORBITING:
            if(app->cursorButtonState[CURSOR_BUTTON_RIGHT] == CURSOR_BUTTON_STATE_DOWN){
                app->orbitCamera.state = ORBIT_CAMERA_STATE_IS_ORBITING;

            }
            break;
        }
}


void do_show_cursor_debug(struct nk_context *ctx, App app){
    if(nk_tree_push(ctx, NK_TREE_TAB, "Cursor", NK_MINIMIZED)){
        char str[20];
        nk_layout_row_static(ctx, 32, 64, 2);
        nk_label(ctx, "x", NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursorPos.x);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "y", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorPos.y);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "normX", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.normCursorPos.x);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "normY", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.normCursorPos.y);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "Ray x", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorRay.x);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "Ray y", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorRay.y);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "Ray z", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorRay.z);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "Ray dx", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorRay.dx);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "Ray dy", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorRay.dy);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "Ray dz", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorRay.dz);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, " px", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorPos3D.x);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "py", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorPos3D.y);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, " pz", NK_TEXT_LEFT);
        sprintf(str, "%f",  app.cursorPos3D.z);
        nk_label(ctx, str , NK_TEXT_LEFT);


        nk_tree_pop(ctx);


    }
}

void do_show_orbit_camera_debug(struct nk_context *ctx, App app){
    if(nk_tree_push(ctx, NK_TREE_TAB, "Orbit Camera", NK_MINIMIZED)){
        char str[20];
        vec3 pos = {0,0,0};
        get_position_orbit_camera(app.orbitCamera, pos);
        nk_layout_row_static(ctx, 32, 64, 2);
        
        nk_label(ctx, "in Vox", NK_TEXT_LEFT);
        sprintf(str, "%d", app.cameraInsideVoxel);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "yaw", NK_TEXT_LEFT);
        sprintf(str, "%f", app.orbitCamera.yaw);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "pitch", NK_TEXT_LEFT);
        sprintf(str, "%f", app.orbitCamera.pitch);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "px", NK_TEXT_LEFT);
        sprintf(str, "%f", pos[0]);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "py", NK_TEXT_LEFT);
        sprintf(str, "%f", pos[1]);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_label(ctx, "pz", NK_TEXT_LEFT);
        sprintf(str, "%f", pos[2]);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_tree_pop(ctx);
    }

}

void do_show_floor_grid_debug(struct nk_context *ctx, App app){
  if(nk_tree_push(ctx, NK_TREE_TAB, "Floor Grid", NK_MINIMIZED)){
        char str1[20], str2[20], str3[20];
        nk_layout_row_static(ctx, 32, 64, 4);
        
        nk_label(ctx, "iVec", NK_TEXT_LEFT);
        sprintf(str1, "%f", app.floorGrid.iVec[0]);
        sprintf(str2, "%f", app.floorGrid.iVec[1]);
        sprintf(str3, "%f", app.floorGrid.iVec[2]);
        nk_label(ctx, str1 , NK_TEXT_LEFT);
        nk_label(ctx, str2 , NK_TEXT_LEFT);
        nk_label(ctx, str3 , NK_TEXT_LEFT);

        nk_label(ctx, "jVec", NK_TEXT_LEFT);
        sprintf(str1, "%f", app.floorGrid.jVec[0]);
        sprintf(str2, "%f", app.floorGrid.jVec[1]);
        sprintf(str3, "%f", app.floorGrid.jVec[2]);
        nk_label(ctx, str1 , NK_TEXT_LEFT);
        nk_label(ctx, str2 , NK_TEXT_LEFT);
        nk_label(ctx, str3 , NK_TEXT_LEFT);

        nk_tree_pop(ctx);
    }
}

void do_show_voxel_debug(struct nk_context *ctx, App app){
    if(nk_tree_push(ctx, NK_TREE_TAB, "Voxel Store", NK_MINIMIZED)){
        char str[20];
        nk_layout_row_static(ctx, 32, 64, 2);
        nk_label(ctx, "Vox Count", NK_TEXT_LEFT);
        sprintf(str, "%d", app.voxelCount);
        nk_label(ctx, str, NK_TEXT_LEFT);

        nk_tree_pop(ctx);
    }
}
void do_show_debug_window(struct nk_context *ctx, App app){
    if(nk_begin(ctx, "Debug Window ", nk_rect(0,0, 128,128),
        NK_WINDOW_BORDER |  NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |NK_WINDOW_TITLE |NK_WINDOW_MINIMIZABLE)){
            do_show_cursor_debug(ctx, app);
            do_show_orbit_camera_debug(ctx, app);
            do_show_floor_grid_debug(ctx, app);
            do_show_voxel_debug(ctx, app);
    }
    nk_end(ctx);
}

void do_show_file_menu(struct nk_context *ctx, App *app){
    struct nk_vec2 size;
    size.x = app->fileMenuSize[0];
    size.y = app->fileMenuSize[1];
    if(nk_menu_begin_label(ctx, "File",NK_TEXT_CENTERED, size)){
        nk_layout_row_dynamic(ctx, 25,1);
        nk_menu_item_label(ctx, "New", NK_TEXT_LEFT);
        nk_menu_item_label(ctx, "Open", NK_TEXT_LEFT);
        nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT);
        if(nk_menu_item_label(ctx, "Quit", NK_TEXT_LEFT)){
            
        }
        nk_menu_end(ctx);
    }
}

void do_show_menu_bar(struct nk_context *ctx, App *app){
    nk_layout_row_static(ctx, app->menuBarHeight, app->menuBarButtonWidth, MENU_BAR_ITEM_COUNT);
    do_show_file_menu(ctx, app);

}

void do_show_property_selector_panel(struct nk_context *ctx, App *app){
    nk_layout_row_push(ctx, app->propertySelectorWidth);
    if(nk_group_begin(ctx, "Property Selector",NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){  
        

        nk_group_end(ctx);
    }

}

void do_show_properties_panel(struct nk_context *ctx, App *app){
    nk_layout_row_push(ctx, app->propertyWidth);
    if(nk_group_begin(ctx, "Properties ",NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
        nk_layout_row_static(ctx, 32, 64, 2);
        int addVoxSelected = (app->voxelManipulationMode == VOXEL_EDIT_ADD_VOXEL) ? 
            nk_true: nk_false;  
        if(nk_select_label(ctx, "addVox",NK_TEXT_ALIGN_LEFT, addVoxSelected)){
            app->voxelManipulationMode = VOXEL_EDIT_ADD_VOXEL;
        }
        int deleteVoxSelected = (app->voxelManipulationMode == VOXEL_EDIT_DELETE_VOXEL) ? 
            nk_true: nk_false;

        if(nk_select_label(ctx, "deleteVox",NK_TEXT_ALIGN_LEFT, deleteVoxSelected)){
            app->voxelManipulationMode = VOXEL_EDIT_DELETE_VOXEL;
        }
        nk_group_end(ctx);
    }
}
void do_show_render_panel(struct nk_context *ctx, App *app){
    nk_layout_row_push(ctx, app->renderPanelWidth);
    
    if(nk_group_begin(ctx, "Render View ",NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
        nk_layout_row_static(ctx,app->renderViewport.height, app->renderViewport.width, 1);
        struct nk_image image = nk_image_id(app->frameTexture);
        
        nk_image_color(ctx,image, nk_white);
        nk_group_end(ctx);
    }


}

void do_show_main_window(struct nk_context *ctx, App *app)
{
    
    nk_style_push_float(ctx, &ctx->style.window.padding.x, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.padding.y, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.spacing.x, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.spacing.y, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.group_padding.x, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.group_padding.y, 0.0f);

    if(nk_begin(ctx, "Main Window", nk_rect(0,0,app->windowViewport.width, app->windowViewport.height),
    NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND |NK_WINDOW_BORDER) ){
        do_show_menu_bar(ctx, app);
        nk_layout_row_begin(ctx, NK_STATIC, app->windowViewport.height - app->menuBarHeight,3);
        do_show_property_selector_panel(ctx, app);
        do_show_properties_panel(ctx, app);
        do_show_render_panel(ctx, app);
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
};



int create_shader_program(GLuint *program, const char *vertShadeSrc, const char *fragShadeSrc){
    GLint success;
    char buffer[1000];
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1, &vertShadeSrc, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(vertexShader,1000,NULL, buffer);
    
    if(!success)
    {   
        printf("Vertex Shader failed to compile\n");
        printf("%s\n",buffer);
        return 1;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1, &fragShadeSrc, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(fragmentShader,1000,NULL, buffer);

    if(!success)
    {   
        printf("Fragment Shader failed to compile\n");
        printf("%s\n",buffer);

        return 1;
    }

    *program = glCreateProgram();   
    glAttachShader(*program, vertexShader);
    glAttachShader(*program, fragmentShader);
    glLinkProgram(*program);
    glGetProgramiv(*program, GL_LINK_STATUS, &success);
    if(!success)
    {
        printf("Shader Program failed to link\n");
        return 1;
    }
    return 0;
}
int load_text_file(const char* path, const char** response){
    FILE *filePointer;
    long fileBytes = 0;
    filePointer = fopen(path,"r");
    if (!filePointer){
        printf("file does not exist\n");
        return 1;
    }
    
    
    fseek(filePointer, 0L, SEEK_END);
    fileBytes = ftell(filePointer);
    fseek(filePointer, 0L, SEEK_SET);	
    
    *response = (const char*)calloc(fileBytes, sizeof(const char));	
    if(*response == NULL){
        printf("not enough memory to allocate file contents");
        return 1;
    }
    fread((char*) *response, sizeof(const char), fileBytes, filePointer);
    return 0;

}
int load_shader_program(GLuint* program,const char* vertexPath,const char* fragmentPath){
    const char* vertexShaderSrc = NULL;
    const char* fragmentShaderSrc = NULL;
    if(load_text_file(vertexPath, &vertexShaderSrc)){
        printf("failed to load vertex shader file\n");
        return 1;
    }

    if(load_text_file(fragmentPath, &fragmentShaderSrc)){
        printf("failed to load fragment shader file\n");
        return 1;
    }
    
    if(create_shader_program(program, vertexShaderSrc, fragmentShaderSrc)){
        printf("failed to create shader program\n");
        return 1;
    }
    return 0;
}

int is_point_inside_voxel(Voxel* voxel, float x, float y, float z){
    if(voxel->size == 1){
        float dx = x - voxel->origin[0];
        float dy = y - voxel->origin[1];
        float dz = z - voxel->origin[2];
        return dx >= 0 && dy >= 0 && dz >= 0 && dx <= 1 && dy <= 1 && dz <=1; 
    }
    int nullCount = 0;
    for(int i = 0; i < 8; i++){
        nullCount += voxel->children[i] == NULL;
    }

    if(nullCount == 8){
        float halfSize = voxel->size * 0.5;
        float minX = voxel->origin[0] - halfSize;
        float maxX = voxel->origin[0] + halfSize;
        float minY = voxel->origin[1] - halfSize;
        float maxY = voxel->origin[1] + halfSize;
        float minZ = voxel->origin[2] - halfSize;
        float maxZ = voxel->origin[2] + halfSize;
        return x >= minX && x <= maxX && y >= minY && y <= maxY && z >=minZ && z <=maxZ;
    }else{
        for(int i = 0; i < 8; i++){
            if(voxel->children[i] != NULL){
                if(is_point_inside_voxel(voxel->children[i], x, y, z)){
                    return 1;
                }
            }
        }
    }
    return 0;
}


void do_delete_voxel_on_ray_hit_voxel(RaycastResult rayResult, App *app){
    int sideNumber = normal_to_box_side(rayResult.hitNormal);
    unsigned int voxelX, voxelY, voxelZ;
    switch (sideNumber)
    {
    case BOX_SIDE_LEFT:
        voxelX = round(rayResult.hitPoint[0]);
        voxelY = rayResult.hitPoint[1];
        voxelZ = rayResult.hitPoint[2];
        break;
    case BOX_SIDE_RIGHT:
        voxelX = round(rayResult.hitPoint[0]) - 1;
        voxelY = rayResult.hitPoint[1];
        voxelZ = rayResult.hitPoint[2];

        break;
    case BOX_SIDE_BOTTOM:
        voxelX = rayResult.hitPoint[0];
        voxelY = round(rayResult.hitPoint[1]);
        voxelZ = rayResult.hitPoint[2];
        break;
    case BOX_SIDE_TOP:
        voxelX = rayResult.hitPoint[0];
        voxelY = round(rayResult.hitPoint[1]) - 1;
        voxelZ = rayResult.hitPoint[2];
        break;
    case BOX_SIDE_FRONT:
        voxelX = rayResult.hitPoint[0];
        voxelY = rayResult.hitPoint[1];
        voxelZ = round(rayResult.hitPoint[2]);
        break;
    case BOX_SIDE_BACK:
        voxelX = rayResult.hitPoint[0];
        voxelY = rayResult.hitPoint[1];
        voxelZ = round(rayResult.hitPoint[2]) - 1;
        break;
    }
    remove_voxel_from_voxel_head(&app->voxelHead, voxelX, voxelY, voxelZ);
}

void do_voxel_edit_delete_voxel(App *app){
    if(app->cursorButtonState[CURSOR_BUTTON_LEFT] == CURSOR_BUTTON_STATE_PRESSED){
        RaycastResult rayResult;
        rayResult.entityHitType = RAY_HIT_NOTHING;
        if(app->voxelHead != NULL){
            do_raycast_voxel_model(&rayResult, *app, app->voxelHead);
        }
        
        if(rayResult.hitPoint[0] >= 0 && 
            rayResult.hitPoint[1] >= 0 &&
            rayResult.hitPoint[2] >=0){
            switch (rayResult.entityHitType)
            {
            case RAY_HIT_ENTITY_VOXEL:
                
                do_delete_voxel_on_ray_hit_voxel(rayResult, app);
                break;
            }

        }
        
    }
}
int is_target_2_closer(vec3 start, vec3 target1, vec3 target2){
    vec3 toTarget1, toTarget2;
    vec3_sub(toTarget1,target2, start);
    vec3_sub(toTarget2, target1, start);
    if(vec3_mul_inner(toTarget2, toTarget2) < vec3_mul_inner(toTarget1, toTarget1)){
        return 1;
    }
    return 0;
}

void do_raycast_y_0(RaycastResult* rayResult, App app){
    float hitX, hitZ;
    if(app.cursorRay.dy != 0 && app.cursorRay.y > 0){
        float d = (-app.cursorRay.y) / app.cursorRay.dy;
        hitX = app.cursorRay.x + app.cursorRay.dx * d;
        hitZ = app.cursorRay.z + app.cursorRay.dz * d;
        vec3 start = {app.cursorRay.x, app.cursorRay.y, app.cursorRay.z};
        vec3 newTarget = {hitX, 0, hitZ};
        int pred = is_target_2_closer(start, rayResult->hitNormal, newTarget);
        if(rayResult->entityHitType == RAY_HIT_NOTHING || pred){
            rayResult->entityHitType = RAY_HIT_Y_PLANE;
            rayResult->hitPoint[0] = hitX;
            rayResult->hitPoint[1] = 0;
            rayResult->hitPoint[2] = hitZ;
            rayResult->hitNormal[0] = 0;
            rayResult->hitNormal[1] = -1 * (app.cursorRay.dy > 0) + 1 * (app.cursorRay.dy < 0);
            rayResult->hitNormal[2] = 0;
        }
    }else{
        rayResult->entityHitType = RAY_HIT_NOTHING;
    }    
}

void boxside_to_normal(int boxSide, vec3 normal){
    switch (boxSide)
    {
    case BOX_SIDE_LEFT:
        normal[0] = -1;
        normal[1] = 0;
        normal[2] = 0;
        break;
    case BOX_SIDE_RIGHT:
        normal[0] = 1;
        normal[1] = 0;
        normal[2] = 0;
        break;
    case BOX_SIDE_BOTTOM:
        normal[0] = 0;
        normal[1] = -1;
        normal[2] = 0;
        break;
    case BOX_SIDE_TOP:
        normal[0] = 0;
        normal[1] = 1;
        normal[2] = 0;
        break;
    case BOX_SIDE_FRONT:
        normal[0] = 0;
        normal[1] = 0;
        normal[2] = -1;
        break;
    case BOX_SIDE_BACK:
        normal[0] = 0;
        normal[1] = 0;
        normal[2] = 1;
        break;
    }
}

int normal_to_box_side(vec3 normal){
    int side = 0;
    side += BOX_SIDE_RIGHT * (normal[0] == 1);
    side += BOX_SIDE_BOTTOM * (normal[-1] == -1);
    side += BOX_SIDE_TOP * (normal[1] == 1);
    side += BOX_SIDE_FRONT * (normal[2] == -1);
    side += BOX_SIDE_BACK * (normal[2] == 1);
    return side;
}
    
void do_raycast_voxel_model(RaycastResult* rayResult, App app, Voxel* voxelHead){
    int hit = 0;
    int sideHit;
    vec3 hitpoint;
    ray_vs_voxel_grid(voxelHead, app.cursorRay, &hit, hitpoint, &sideHit);

    if(hit){
        vec3 start;
        int pred = is_target_2_closer(start, rayResult->hitPoint, hitpoint);
        if(rayResult->entityHitType == RAY_HIT_NOTHING || pred){
            rayResult->entityHitType = RAY_HIT_ENTITY_VOXEL;
            rayResult->hitPoint[0] = hitpoint[0];
            rayResult->hitPoint[1] = hitpoint[1];
            rayResult->hitPoint[2] = hitpoint[2];
            boxside_to_normal(sideHit, rayResult->hitNormal);
        }
    }

} 

void do_add_voxel_on_ray_hit_voxel(RaycastResult rayResult, App *app){
    int sideNumber = normal_to_box_side(rayResult.hitNormal);
    unsigned int voxelX, voxelY, voxelZ;
    switch (sideNumber)
    {
    case BOX_SIDE_LEFT:
        voxelX = round(rayResult.hitPoint[0]);
        voxelY = rayResult.hitPoint[1];
        voxelZ = rayResult.hitPoint[2];
        break;
    case BOX_SIDE_RIGHT:
        voxelX = round(rayResult.hitPoint[0]);
        voxelY = rayResult.hitPoint[1];
        voxelZ = rayResult.hitPoint[2];

        break;
    case BOX_SIDE_BOTTOM:
        voxelX = rayResult.hitPoint[0];
        voxelY = round(rayResult.hitPoint[1]);
        voxelZ = rayResult.hitPoint[2];
        break;
    case BOX_SIDE_TOP:
        voxelX = rayResult.hitPoint[0];
        voxelY = round(rayResult.hitPoint[1]);
        voxelZ = rayResult.hitPoint[2];
        break;
    case BOX_SIDE_FRONT:
        voxelX = rayResult.hitPoint[0];
        voxelY = rayResult.hitPoint[1];
        voxelZ = round(rayResult.hitPoint[2]);
        break;
    case BOX_SIDE_BACK:
        voxelX = rayResult.hitPoint[0];
        voxelY = rayResult.hitPoint[1];
        voxelZ = round(rayResult.hitPoint[2]);
        break;
    }

    add_voxel_child_to_voxel_head(&app->voxelHead, voxelX, voxelY, voxelZ);
}

void do_voxel_edit_add_voxel(App *app){
    if(app->cursorButtonState[CURSOR_BUTTON_LEFT] == CURSOR_BUTTON_STATE_PRESSED){
        RaycastResult rayResult;
        rayResult.entityHitType = RAY_HIT_NOTHING;
        do_raycast_y_0(&rayResult, *app);
        if(app->voxelHead != NULL){
            do_raycast_voxel_model(&rayResult, *app, app->voxelHead);

        }
        
        if(rayResult.hitPoint[0] >= 0 && 
            rayResult.hitPoint[1] >= 0 &&
            rayResult.hitPoint[2] >=0){
            switch (rayResult.entityHitType)
            {
            case RAY_HIT_Y_PLANE:
                add_voxel_child_to_voxel_head(&app->voxelHead, 
                (VoxInt)rayResult.hitPoint[0], (VoxInt)0, (VoxInt)rayResult.hitPoint[2]);
                break;
            case RAY_HIT_ENTITY_VOXEL:
                do_add_voxel_on_ray_hit_voxel(rayResult, app);
                break;
            }

        }
        
    }
}



AppGlobals appGlobals;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    appGlobals.xScroll = xoffset;
    appGlobals.yScroll = yoffset;
}

void init_globals(AppGlobals* appGlobals){
    appGlobals->xScroll = 0;
    appGlobals->yScroll = 0;
}

void app_globals_zero_scrolls(AppGlobals *globals){
    globals->xScroll = 0;
    globals->yScroll = 0;

}

int main(int argc, char const *argv[]){
    int appMode; 
    struct nk_glfw glfw = {0};
    struct nk_context *ctx;
    GLFWwindow *window;
    App app;

    init_globals(&appGlobals);
    appMode = APP_MODE_DEFAULT ;
    if (strcmp(argv[1], "debug") == 0)
    {
        appMode = APP_MODE_DEBUG;
    }
    
        
    if (!glfwInit()){
        return -1;
    }


    window = glfwCreateWindow(640, 480, "Voxel Lab", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        init_app_viewport(&app.windowViewport,0,0,width,height);

    }
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        init_app_viewport(&app.renderViewport,0,0,width,height);
    }


    {
        vec3 pos= {0,0,0};
        init_orbit_camera(&app.orbitCamera, pos, 0,0,M_2_PI,1.0,100.0,app.renderViewport.ratio,10.0);
    }

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }  
    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);    
    glfwSetScrollCallback(window, scroll_callback);  

    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);
        nk_glfw3_font_stash_end(&glfw);
    }            

    {
        double x, y;   
        glfwGetCursorPos(window, &x, &y);
        app.cursorPos.x = (float)x;
        app.cursorPos.y = (float)y;
        app.cursorDelta.x = 0;
        app.cursorDelta.y = 0;
        
    }

    for(int i = 0; i < CURSOR_BUTTON_COUNT; i++){
        app.cursorButtonState[i] = CURSOR_BUTTON_STATE_UP;
    }
    
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        init_app_viewport(&app.renderViewport,0,0,width, height);
    }
    {
        set_i_vec_grid(&app.floorGrid, 1,0,0);
        set_j_vec_grid(&app.floorGrid, 0,0,1);
        set_pos_grid(&app.floorGrid, 0,0,0);
        app.floorGrid.iCount = 32;
        app.floorGrid.jCount = 32;
    }


    {        
        app.floorGridVertexCount = get_vertex_count(app.floorGrid);    
        app.floorGridModel = (GridVertex*)calloc(app.floorGridVertexCount,sizeof(GridVertex));

        int index = 0;
        for(int i = 0; i <= app.floorGrid.iCount; i++){
            vec3 vert1, vert2;
            vec3_scale(vert1,app.floorGrid.iVec, i);
            init_grid_vertex(&app.floorGridModel[index], vert1[0], vert1[1], vert1[2],1.0,1.0,1.0, 1.0);
            index++;
            vec3_scale(vert2, app.floorGrid.jVec, app.floorGrid.jCount );
            vec3_add(vert2, vert2, vert1);
            init_grid_vertex(&app.floorGridModel[index], vert2[0], vert2[1], vert2[2],1.0,1.0,1.0, 1.0);
            index++; 
        }
        for(int i = 0; i <= app.floorGrid.jCount; i++){
            vec3 vert1, vert2;
            vec3_scale(vert1,app.floorGrid.jVec, i);
            init_grid_vertex(&app.floorGridModel[index], vert1[0], vert1[1], vert1[2],1.0,1.0,1.0, 1.0);
            index++;
            vec3_scale(vert2, app.floorGrid.iVec, app.floorGrid.iCount);
            vec3_add(vert2, vert2, vert1);
            init_grid_vertex(&app.floorGridModel[index], vert2[0], vert2[1], vert2[2],1.0,1.0,1.0, 1.0);
            index++;
        }
    }
    if(load_shader_program(&app.shaderID[SHADER_ID_GRID_SHADER],"res/shaders/grid_vertex.glsl", "res/shaders/grid_fragment.glsl")){
        printf("failed to load shader\n");
        return 1;
    }

    if(load_shader_program(&app.shaderID[SHADER_ID_VOXEL_SHADER],"res/shaders/voxel_vertex.glsl","res/shaders/voxel_fragment.glsl")){
        printf("failed to load nessessary shader\n");
        return 1;
    }
    
    glGenBuffers(1, &app.floorGridVbo);
    glBindBuffer(GL_ARRAY_BUFFER, app.floorGridVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GridVertex) * app.floorGridVertexCount, app.floorGridModel, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &app.voxelModelVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, app.voxelModelVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &app.vertexModelIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.vertexModelIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenFramebuffers(1, &app.frameBuffer);
    glGenTextures(1, &app.frameTexture);
    glGenTextures(1, &app.frameDepthTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, app.frameBuffer);
    glBindTexture(GL_TEXTURE_2D, app.frameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, app.renderViewport.width, app.renderViewport.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, app.frameTexture, 0);  
    glBindTexture(GL_TEXTURE_2D, app.frameDepthTexture);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, app.renderViewport.width,app.renderViewport.height, 
        0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, app.frameDepthTexture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("Framebuffer is not complete!");
    }

    init_color_rgba_f(&app.colorData[COLOR_DATA_BACKGROUND_COLOR], 0.5f,0.5f,0.5f,1.0f);
    app.orbitCamera.state = ORBIT_CAMERA_STATE_NOT_ORBITING;
    app.propertySelectorWidth = 64;
    app.propertyWidth = 128;
    app.appCursorFocus = APP_CURSOR_FOCUS_GUI;
    app.cursorRayHit = 0;
    app.voxelCount = 0;

    app.voxelModelVerticies = (VoxelVertex*) malloc(sizeof(VoxelVertex));
    app.voxelModelIndicies = (unsigned int*) malloc(sizeof(unsigned int));
    
    app.voxelManipulationMode = VOXEL_EDIT_ADD_VOXEL;
    while (!glfwWindowShouldClose(window)){  

        app.updateRequest = DO_UPDATE_VIEW_MAT + DO_UPDATE_PROJ_MAT + 
            DO_UPDATE_INV_MAT + DO_UPDATE_NORMALIZED_CURSOR + 
            DO_UPDATE_CURSOR_RAY + DO_RAYCAST_VOXEL_MODEL + DO_RAYCAST_GRID;
        
        {   
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            app.windowViewport.width = (float)width;
            app.windowViewport.height = (float)height;
            app.windowViewport.ratio = width / (float)height;
        }
        

        app.menuBarHeight = 24;
        app.menuBarButtonWidth = 32;
        app.renderPanelWidth = app.windowViewport.width - app.propertySelectorWidth - app.propertyWidth;
        
        init_app_viewport(
            &app.renderViewport,
            app.propertyWidth + app.propertySelectorWidth, 
            app.menuBarHeight,
            app.renderPanelWidth,
            app.windowViewport.height - app.menuBarHeight
        );
        app.renderViewport.ratio = app.renderViewport.width / app.renderViewport.height;
        app.fileMenuSize[0] = 64;
        app.fileMenuSize[1] = 64;

        if(app.updateRequest | DO_UPDATE_NORMALIZED_CURSOR)
        {
            double x, y;
            float normX, normY;  
            glfwGetCursorPos(window, &x ,&y);
            app.cursorDelta.x = x - app.cursorPos.x;
            app.cursorDelta.y = y - app.cursorPos.y;
            app.cursorPos.x = x;
            app.cursorPos.y = y;
            get_norm_position_from_cursor(&normX, &normY, app.cursorPos.x, app.cursorPos.y,
            app.renderViewport.x, app.renderViewport.y, 
            app.renderViewport.width, app.renderViewport.height
            );
            app.normCursorPos.x = normX;
            app.normCursorPos.y = normY;         
        }
 
        app.leftBtn = glfwGetMouseButton(window, 0);
        app.rightBtn = glfwGetMouseButton(window, 1);        
        do_cursor_event(&app);
        do_orbit_camera_event(&app);
        app.orbitCamera.distance += appGlobals.yScroll;

        
             
       
        app.orbitCamera.aspectRat = app.renderViewport.ratio;

        if(app.updateRequest | DO_UPDATE_PROJ_MAT){
            get_projection_matrix_orbit_camera(app.orbitCamera, app.projMat);
        }
        if(app.updateRequest | DO_UPDATE_VIEW_MAT){
            get_view_matrix_orbit_camera(app.orbitCamera, app.viewMat);

        }
        if(app.updateRequest | DO_UPDATE_INV_MAT){
            mat4x4 projView;
            get_position_orbit_camera(app.orbitCamera, app.orbitCameraPosition);
            mat4x4_mul(projView, app.projMat, app.viewMat);
            mat4x4_invert(app.inverseMat, projView);
            
        }

        if(app.updateRequest | DO_UPDATE_CURSOR_RAY)
        {  
            vec3 rayDirection;
            vec4 worldSpaceTarget, screenSpaceTarget;
            vec3 worldTarget3D; 
            float div;
            screenSpaceTarget[0] = app.normCursorPos.x;
            screenSpaceTarget[1] = -app.normCursorPos.y;
            screenSpaceTarget[2] = -1.0;
            screenSpaceTarget[3] = 1.0;
            mat4x4_mul_vec4(worldSpaceTarget, app.inverseMat, screenSpaceTarget);
            div = 1 / worldSpaceTarget[3];
            worldTarget3D[0] = worldSpaceTarget[0] * div;
            worldTarget3D[1] = worldSpaceTarget[1] * div;
            worldTarget3D[2] = worldSpaceTarget[2] * div;
            vec3_sub(rayDirection, worldTarget3D, app.orbitCameraPosition);
            vec3_norm(rayDirection, rayDirection);
            init_ray(&app.cursorRay, 
                app.orbitCameraPosition[0], app.orbitCameraPosition[1],app.orbitCameraPosition[2],
                rayDirection[0],rayDirection[1],rayDirection[2]
            ); 
        } 

        switch (app.voxelManipulationMode)
        {
        case VOXEL_EDIT_ADD_VOXEL:
            do_voxel_edit_add_voxel(&app);
            break;
        case VOXEL_EDIT_DELETE_VOXEL :
            do_voxel_edit_delete_voxel(&app);
            break;
        }
        
        if(app.cursorButtonState[CURSOR_BUTTON_LEFT] == CURSOR_BUTTON_STATE_PRESSED){

            if(app.voxelHead != NULL){
                app.voxelCount = count_voxels(app.voxelHead);
            }
            
            app.voxelModelVerticies = (VoxelVertex*) realloc(app.voxelModelVerticies, sizeof(VoxelVertex) * app.voxelCount * VERTEX_PER_CUBE);
            app.voxelModelIndicies = (unsigned int*) realloc(app.voxelModelIndicies, sizeof(unsigned int) * INDICIES_PER_CUBE * app.voxelCount);

            Voxel* voxelLeaves[app.voxelCount];

            int index = 0;
            if(app.voxelHead != NULL){
                get_leaf_voxels(app.voxelHead, voxelLeaves, &index);
            }
            
            {
                unsigned int element = 0;
                int vertexIndex = 0;
                int elementIndex = 0;
                for(int i = 0; i < app.voxelCount; i++){
                    float x, y, z;
                    int size;
                    x = voxelLeaves[i]->origin[0];
                    y = voxelLeaves[i]->origin[1];
                    z = voxelLeaves[i]->origin[2];
                    size = voxelLeaves[i]->size;                    
                    if(size != 1){
                        int halfSize = size >> 1;
                        x -= halfSize;
                        y -= halfSize;
                        z -= halfSize;
                    }
                    init_voxel_vertex(&app.voxelModelVerticies[vertexIndex], x, y, z, 0.0, 1.0, 0.0, 1.0);
                    vertexIndex++;
                    init_voxel_vertex(&app.voxelModelVerticies[vertexIndex], x, y + size, z, 0.0, 1.0, 0.0, 1.0);
                    vertexIndex++;
                    init_voxel_vertex(&app.voxelModelVerticies[vertexIndex], x + size, y + size, z, 0.0, 1.0, 0.0, 1.0);
                    vertexIndex++;
                    init_voxel_vertex(&app.voxelModelVerticies[vertexIndex], x + size, y, z, 0.0, 1.0, 0.0, 1.0);
                    vertexIndex++;
                    init_voxel_vertex(&app.voxelModelVerticies[vertexIndex], x, y, z + size, 0.0, 1.0, 0.0, 1.0);
                    vertexIndex++;
                    init_voxel_vertex(&app.voxelModelVerticies[vertexIndex], x, y + size, z + size, 0.0, 1.0, 0.0, 1.0);
                    vertexIndex++;
                    init_voxel_vertex(&app.voxelModelVerticies[vertexIndex], x + size, y + size, z + size, 0.0, 1.0, 0.0, 1.0);
                    vertexIndex++;
                    init_voxel_vertex(&app.voxelModelVerticies[vertexIndex], x + size, y, z + size, 0.0, 1.0, 0.0, 1.0);
                    vertexIndex++;

                    app.voxelModelIndicies[elementIndex] = element;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 1;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 3;   
                    elementIndex++;

                    app.voxelModelIndicies[elementIndex] = element + 1;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 2;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 3; 
                    elementIndex++;

                    
                    app.voxelModelIndicies[elementIndex] = element + 4;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 5;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element;   
                    elementIndex++;

                    app.voxelModelIndicies[elementIndex] = element + 5;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 1;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element;     
                    elementIndex++;


                    app.voxelModelIndicies[elementIndex] = element + 1;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 5;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 2;   
                    elementIndex++;

                    app.voxelModelIndicies[elementIndex] = element + 5;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 6;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 2;
                    elementIndex++;

                    
                    app.voxelModelIndicies[elementIndex] = element + 4;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 0;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 7;   
                    elementIndex++;

                    app.voxelModelIndicies[elementIndex] = element;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 3;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 7;   
                    elementIndex++;


                    app.voxelModelIndicies[elementIndex] = element + 3;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 2;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 7;   
                    elementIndex++;

                    app.voxelModelIndicies[elementIndex] = element + 2;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 6;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 7;
                    elementIndex++;


                    app.voxelModelIndicies[elementIndex] = element + 7;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 6;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 5;   
                    elementIndex++;

                    app.voxelModelIndicies[elementIndex] = element + 4;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 5;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 7;      
                    elementIndex++;
                    element += 8;

                }
                glBindBuffer(GL_ARRAY_BUFFER, app.voxelModelVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(VoxelVertex) * VERTEX_PER_CUBE * app.voxelCount, app.voxelModelVerticies, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.vertexModelIndexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * INDICIES_PER_CUBE * app.voxelCount, app.voxelModelIndicies, GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
          
            }

        }
        

        app_globals_zero_scrolls(&appGlobals);
        glfwPollEvents();  

        glBindTexture(GL_TEXTURE_2D, app.frameTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, app.renderViewport.width, app.renderViewport.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, app.frameDepthTexture); 
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, app.renderViewport.width,app.renderViewport.height, 
            0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
        );
        glViewport(0,0, app.renderViewport.width, app.renderViewport.height);

        glBindFramebuffer(GL_FRAMEBUFFER, app.frameBuffer);
        {
            ColorRGBAf col = app.colorData[COLOR_DATA_BACKGROUND_COLOR];
            glClearColor(col.r, col.g, col.b, col.a);
        }  

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        glBindBuffer(GL_ARRAY_BUFFER, app.floorGridVbo);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7* sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE,7 * sizeof(float),(void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glUseProgram(app.shaderID[SHADER_ID_GRID_SHADER]);
        glUniformMatrix4fv(glGetUniformLocation(app.shaderID[SHADER_ID_GRID_SHADER],"uProjMat"),1,GL_FALSE, (float*)app.projMat);
        glUniformMatrix4fv(glGetUniformLocation(app.shaderID[SHADER_ID_GRID_SHADER],"uViewMat"),1,GL_FALSE, (float*)app.viewMat);
        glDrawArrays(GL_LINES,0, app.floorGridVertexCount);
        
        glBindBuffer(GL_ARRAY_BUFFER, app.voxelModelVertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.vertexModelIndexBuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7* sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glUseProgram(app.shaderID[SHADER_ID_VOXEL_SHADER]);
        glUniformMatrix4fv(glGetUniformLocation(app.shaderID[SHADER_ID_VOXEL_SHADER],"uProjMat"),1,GL_FALSE, (float*)app.projMat);
        glUniformMatrix4fv(glGetUniformLocation(app.shaderID[SHADER_ID_VOXEL_SHADER],"uViewMat"),1,GL_FALSE, (float*)app.viewMat);
        glDrawElements(GL_TRIANGLES, INDICIES_PER_CUBE * app.voxelCount, GL_UNSIGNED_INT, 0);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        nk_glfw3_new_frame(&glfw);
        do_show_main_window(ctx, &app);
        if(appMode == APP_MODE_DEBUG){    
            do_show_debug_window(ctx, app);
        }
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        
        glfwSwapBuffers(window);

    }     
    nk_glfw3_shutdown(&glfw);
    return 0;

}