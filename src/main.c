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

#include "../include/color.h"
#include "../include/cursor.h"
#include "../include/grid.h"
#include "../include/orbit_camera.h"
#include "../include/menu_bar.h"
#include "../include/vector2.h"
#include "../include/vector3.h"
#include "../include/app_viewport.h"
#include "../include/voxel.h"
#include "../include/aabb.h"
#include "../include/ray.h"

enum app_cursor_focus{APP_CURSOR_FOCUS_GUI,APP_CURSOR_FOCUS_RENDER_VIEWPORT, APP_CURSOR_FOCUS_OUTSIDE_WINDOW};
enum app_mode{APP_MODE_DEBUG, APP_MODE_DEFAULT};
enum box_sides{BOX_SIDE_LEFT, BOX_SIDE_RIGHT, BOX_SIDE_BOTTOM, BOX_SIDE_TOP, BOX_SIDE_FRONT, BOX_SIDE_BACK};

typedef struct AppGlobals{
    float yScroll;
    float xScroll;
}AppGlobals;

typedef struct VoxelVertex{
    float x, y, z;
    float r, g, b, a;
}VoxelVertex;

void init_voxel_vertex(VoxelVertex* voxelVertex, float x, float y, float z, float r, float g, float b, float a){
    voxelVertex->x = x;
    voxelVertex->y = y;
    voxelVertex->z = z;
    voxelVertex->r = r;
    voxelVertex->g = g;
    voxelVertex->b = b;
    voxelVertex->a = a;
}

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
    int projMatShouldUpdate, viewMatShouldUpdate, inverseMatShouldUpdate;
    ColorRGBAf sceneBackColor;
    AppViewport renderViewport, windowViewport;
    float menuBarHeight;
    float menuBarButtonWidth, propertySelectorWidth, propertyWidth, renderPanelWidth;
    vec2 fileMenuSize;
    Grid floorGrid;
    GridVertex* floorGridModel;
    int floorGridVertexCount;
    ColorRGBAf floorGridColor;
    GLuint floorGridVbo, gridShader, voxelShader, frameTexture, frameDepthTexture, frameBuffer,
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

int aabb_contains_point(Aabb box, float x, float y, float z){
    int minX = box.x;
    int maxX = box.x + box.width;
    int minY = box.y;
    int maxY = box.y + box.height;
    int minZ = box.z;
    int maxZ = box.z + box.depth;
    return x >= minX && x < maxX && y >= minY && y < maxY &&
        z >= minZ && z < maxZ;
}

Aabb voxel_to_aabb(Voxel voxel){
    Aabb aabb;
    float halfSize = voxel.size * 0.5;
    int pred = halfSize *(voxel.size!=1);
    aabb.x = voxel.origin[0] - pred;
    aabb.y = voxel.origin[1] - pred;
    aabb.z = voxel.origin[2] - pred;
    aabb.width = voxel.size;
    aabb.height = voxel.size;
    aabb.depth = voxel.size;

    return aabb;
}

void  ray_vs_aabb(Aabb box, Ray ray, vec3 hitpoint, int *didHit, int *boxSide){
    *didHit = 0;
    float left = box.x;
    float right = box.x + box.width;
    float bottom = box.y;
    float top = box.y + box.height;
    float front = box.z ;
    float back = box.z + box.depth;
    vec3 rayStart = {ray.x, ray.y, ray.z};
    vec3 rayDirection = {ray.dx, ray.dy, ray.dz};
    int bestDistance = 1000000000;
    vec3 bestHitPoint;
    int leftTarget = ray.dx > 0;
    int rightTarget = ray.dx < 0;
    int bottomTarget = ray.dy > 0;
    int topTarget = ray.dy < 0;
    int frontTarget = ray.dz > 0; 
    int backTarget = ray.dz < 0; 

    if(leftTarget){
        vec3 hitPointLeft;        
        float d = (left - ray.x) / ray.dx;        
        vec3_scale(hitPointLeft, rayDirection, d);
        vec3_add(hitPointLeft,hitPointLeft, rayStart);
        float hitX = hitPointLeft[0];
        float hitY = hitPointLeft[1];
        float hitZ = hitPointLeft[2];
        int hit = hitY >= bottom && hitY < top && hitZ >= front && hitZ < back;
        if(hit){
            bestHitPoint[0] = hitX;
            bestHitPoint[1] = hitY;
            bestHitPoint[2] = hitZ;
            bestDistance = d;
            *didHit = 1;
            *boxSide = BOX_SIDE_LEFT;
        }
           
    }else if(rightTarget){
        vec3 hitPointRight;        
        float d = (right - ray.x) / ray.dx;
        vec3_scale(hitPointRight, rayDirection, d);
        vec3_add(hitPointRight, hitPointRight, rayStart);
        float hitX = hitPointRight[0];
        float hitY = hitPointRight[1];
        float hitZ = hitPointRight[2];
        int hit = hitY >= bottom && hitY < top && hitZ >= front && hitZ < back;
        if(hit){
            bestHitPoint[0] = hitX;
            bestHitPoint[1] = hitY;
            bestHitPoint[2] = hitZ;
            bestDistance = d;
            *didHit = 1;
            *boxSide = BOX_SIDE_RIGHT;


        }
        
    }
    
    
    if(frontTarget){
        vec3 hitPointFront;        
        float d = (front - ray.z)/ ray.dz;
        if(d < bestDistance){
            vec3_scale(hitPointFront, rayDirection, d);
            vec3_add(hitPointFront, hitPointFront, rayStart);
            float hitX = hitPointFront[0];
            float hitY = hitPointFront[1];
            float hitZ = hitPointFront[2];
            int hit = hitX >= left && hitX < right && hitY >= bottom && hitY < top;
            if(hit){
                bestHitPoint[0] = hitX;
                bestHitPoint[1] = hitY;
                bestHitPoint[2] = hitZ;
                bestDistance = d;
                *didHit = 1;
                *boxSide = BOX_SIDE_FRONT;

            }
        }


    }else if(backTarget){
        vec3 hitPointBack;        
        float d = (back - ray.z)/ ray.dz;
        if(d < bestDistance){
            vec3_scale(hitPointBack, rayDirection, d);
            vec3_add(hitPointBack, hitPointBack, rayStart);
            float hitX = hitPointBack[0];
            float hitY = hitPointBack[1];
            float hitZ = hitPointBack[2];
            int hit = hitX >= left && hitX < right && hitY >= bottom && hitY < top;
            if(hit){
                bestHitPoint[0] = hitX;
                bestHitPoint[1] = hitY;
                bestHitPoint[2] = hitZ;
                bestDistance = d;
                *didHit = 1;
                *boxSide = BOX_SIDE_BACK;
            }
        }
    }
   

    if(bottomTarget){
        vec3 hitPointBottom;        
        float d = (bottom - ray.y)/ ray.dy;
        if(d <= bestDistance){
            vec3_scale(hitPointBottom, rayDirection, d);
            vec3_add(hitPointBottom, hitPointBottom, rayStart);
            float hitX = hitPointBottom[0];
            float hitY = hitPointBottom[1];
            float hitZ = hitPointBottom[2];
            int hit = hitX >= left && hitX < right && hitZ >= front && hitZ < back;
            if(hit){
                bestHitPoint[0] = hitX;
                bestHitPoint[1] = hitY;
                bestHitPoint[2] = hitZ;
                bestDistance = d;
                *didHit = 1;
                *boxSide = BOX_SIDE_BOTTOM;
            }
        }
    }else if(topTarget){
        vec3 hitPointTop;        
        float d = (top - ray.y)/ ray.dy;
        if(d <= bestDistance){
            vec3_scale(hitPointTop, rayDirection, d);
            vec3_add(hitPointTop, hitPointTop, rayStart);
            float hitX = hitPointTop[0];
            float hitY = hitPointTop[1];
            float hitZ = hitPointTop[2];
            int hit = hitX >= left && hitX < right && hitZ >= front && hitZ < back;
            if(hit){
                bestHitPoint[0] = hitX;
                bestHitPoint[1] = hitY;
                bestHitPoint[2] = hitZ;
                bestDistance = d;
                *didHit = 1;
                *boxSide = BOX_SIDE_TOP;
            }
        }
    }
    if(*didHit){
        hitpoint[0] = bestHitPoint[0];
        hitpoint[1] = bestHitPoint[1];
        hitpoint[2] = bestHitPoint[2];
    }

}

void ray_vs_voxel(Voxel* voxel, Ray ray, vec3 hitpoint, int *didHit, int *hitSide){
    if(voxel->size == 1){
        Aabb voxBox;
        vec3 hitpoint;
        voxBox = voxel_to_aabb(*voxel);
        int boxHit = 0;
        int boxSide;
        vec3 boxHitPoint;
        ray_vs_aabb(voxBox, ray, boxHitPoint, &boxHit, &boxSide);
        
        if(boxHit){
            vec3 rayStart = {ray.x, ray.y, ray.z};
            vec3 vectorToHitpoint, vectorToBox;
            vec3_sub(vectorToHitpoint, hitpoint, rayStart);
            vec3_sub(vectorToBox, boxHitPoint, rayStart);
            float distanceToBox = vec3_mul_inner(vectorToBox, vectorToBox);
            float distanceToHitpoint = vec3_mul_inner(vectorToHitpoint, vectorToHitpoint);
        
            if(distanceToBox < distanceToHitpoint || *didHit == 0){
                hitpoint[0] = boxHitPoint[0];
                hitpoint[1] = boxHitPoint[1];
                hitpoint[2] = boxHitPoint[2];
                *didHit = 1;
                *hitSide = boxSide;
                
            }
        }
    }

    int nullCount = 0;
    for(int i = 0; i < 8; i++){
        nullCount += voxel->children[i] == NULL;
    }

    if(nullCount == 8){ 
        Aabb voxBox;
        vec3 hitpoint;
        voxBox = voxel_to_aabb(*voxel);
        int boxHit;
        int boxSide;
        vec3 boxHitPoint;

        ray_vs_aabb(voxBox, ray, hitpoint, &boxHit, &boxSide);
        if(boxHit){
            vec3 rayStart = {ray.x, ray.y, ray.z};
            vec3 vectorToHitpoint, vectorToBox;
            vec3_sub(vectorToHitpoint, hitpoint, rayStart);
            vec3_sub(vectorToBox, boxHitPoint, rayStart);
            float distanceToBox = vec3_mul_inner(vectorToBox, vectorToBox);
            float distanceToHitpoint = vec3_mul_inner(vectorToHitpoint, vectorToHitpoint);
        
            if(distanceToBox < distanceToHitpoint || *didHit){
                hitpoint[0] = boxHitPoint[0];
                hitpoint[1] = boxHitPoint[1];
                hitpoint[2] = boxHitPoint[2];
                *didHit = 1;
                *hitSide = boxSide;
                
            }
        }
     
    }

    for(int i = 0; i < 8; i++){
        if(voxel->children[i] != NULL){
            Aabb box;
            box = voxel_to_aabb(*(voxel->children[i]));
            if(aabb_contains_point(box, ray.x, ray.y, ray.z)){

                ray_vs_voxel(voxel->children[i], ray, hitpoint, didHit, hitSide);
            }else{
                int hit;
                int side;
                ray_vs_aabb(box, ray, hitpoint,&hit, &side);
                if(hit){
                    ray_vs_voxel(voxel->children[i], ray, hitpoint, didHit, hitSide);
                }
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
        app.windowViewport.width = width;
        app.windowViewport.height = height;
        app.windowViewport.ratio = width / (float)height;
    }
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        app.renderViewport.width = width;
        app.renderViewport.height = height;
        app.renderViewport.ratio = width / (float)height;
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
    if(load_shader_program(&app.gridShader,"res/shaders/grid_vertex.glsl", "res/shaders/grid_fragment.glsl")){
        printf("failed to load shader\n");
        return 1;
    }

    if(load_shader_program(&app.voxelShader,"res/shaders/voxel_vertex.glsl","res/shaders/voxel_fragment.glsl")){
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

    init_color_rgba_f(&app.sceneBackColor, 0.5f,0.5f,0.5f,1.0f);
    app.orbitCamera.state = ORBIT_CAMERA_STATE_NOT_ORBITING;
    app.propertySelectorWidth = 64;
    app.propertyWidth = 128;
    app.appCursorFocus = APP_CURSOR_FOCUS_GUI;
    app.cursorRayHit = 0;
    app.voxelCount = 0;

    app.voxelModelVerticies = (VoxelVertex*) malloc(sizeof(VoxelVertex));
    app.voxelModelIndicies = (unsigned int*) malloc(sizeof(unsigned int));
    while (!glfwWindowShouldClose(window)){  
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
        
        app.renderViewport.x = app.propertyWidth + app.propertySelectorWidth;
        app.renderViewport.y = app.menuBarHeight;
        app.renderViewport.width = app.renderPanelWidth;
        app.renderViewport.height = app.windowViewport.height - app.menuBarHeight;
        app.renderViewport.ratio = app.renderViewport.width / app.renderViewport.height;
        app.fileMenuSize[0] = 64;
        app.fileMenuSize[1] = 64;
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
        {
            int inRenderView = (app.normCursorPos.x >= -1.0 && app.normCursorPos.x < 1.0 && 
                app.normCursorPos.y >= -1.0 && app.normCursorPos.y <= 1.0);
            
            app.appCursorFocus = APP_CURSOR_FOCUS_RENDER_VIEWPORT * inRenderView + APP_CURSOR_FOCUS_GUI * !inRenderView;
        }
        app.leftBtn = glfwGetMouseButton(window, 0);
        app.rightBtn = glfwGetMouseButton(window, 1);        
        do_cursor_event(&app);
        do_orbit_camera_event(&app);
        app.orbitCamera.distance += appGlobals.yScroll;

        
        glViewport(0,0, app.renderViewport.width, app.renderViewport.height);
             
        
        
        app.orbitCamera.aspectRat = app.renderViewport.ratio;
        get_projection_matrix_orbit_camera(app.orbitCamera, app.projMat);
        get_view_matrix_orbit_camera(app.orbitCamera, app.viewMat);
        get_position_orbit_camera(app.orbitCamera, app.orbitCameraPosition);
        {
            mat4x4 projView;
            mat4x4_mul(projView, app.projMat, app.viewMat);
            mat4x4_invert(app.inverseMat, projView);
        }
        if(app.voxelHead != NULL){
            app.cameraInsideVoxel = is_point_inside_voxel(app.voxelHead, 
                app.orbitCameraPosition[0],
                app.orbitCameraPosition[1],
                app.orbitCameraPosition[2]
            );
        }
        
        {  
            vec3 rayDirection;
            vec4 target ,targ = {app.normCursorPos.x, -app.normCursorPos.y, -1.0, 1.0};
            mat4x4_mul_vec4(target, app.inverseMat, targ);
            float div = 1 / target[3];
            vec3 target3 = {target[0] * div, target[1] * div, target[2] * div};
            vec3_sub(rayDirection, target3, app.orbitCameraPosition);
            vec3_norm(rayDirection, rayDirection);
            init_ray(&app.cursorRay, 
                app.orbitCameraPosition[0], app.orbitCameraPosition[1],app.orbitCameraPosition[2],
                rayDirection[0],rayDirection[1],rayDirection[2]
            ); 
        }    
            
            
        int voxPassed = 0;
        {
            /*Voxel Test*/
            vec3 voxHitPoint;
            int sideHit;
            if(app.voxelHead != NULL){
                ray_vs_voxel(app.voxelHead, app.cursorRay, voxHitPoint, &voxPassed, &sideHit);

            }
            if(voxPassed){
                app.cursorPos3D.x = voxHitPoint[0];
                app.cursorPos3D.y = voxHitPoint[1];
                app.cursorPos3D.z = voxHitPoint[2];
            }
        }
        if(!voxPassed)
        {
            
            float px, pz;
            float py = 0; 
            //floor Test
            if(app.cursorRay.dy == 0){
                app.cursorRayHit = 0;
            }else{

                app.cursorRayHit = 1;
                float d = (-app.orbitCameraPosition[1]) / app.cursorRay.dy;
                px = app.cursorRay.x + app.cursorRay.dx * d;
                pz = app.cursorRay.z + app.cursorRay.dz * d;
                app.cursorRayHit = 1;
                app.cursorPos3D.x = px;
                app.cursorPos3D.y = py;
                app.cursorPos3D.z = pz;
            }  
                 
        }
            //ray from voxel model
           
        //add voxel 
        if(app.cursorButtonState[CURSOR_BUTTON_LEFT] == CURSOR_BUTTON_STATE_PRESSED){
            if(app.cursorRayHit){
                int x,y,z;
                x = app.cursorPos3D.x;
                y = app.cursorPos3D.y;
                z = app.cursorPos3D.z;
                if(x >= 0 && y >= 0 && z >= 0){
                    VoxInt ux,uy,uz;
                    ux = x; uy = y; uz = z;
                    add_voxel_child_to_voxel_head(&app.voxelHead,ux,uy,uz);
                }
            }
            if(app.voxelHead != NULL){
                app.voxelCount = count_voxels(app.voxelHead);
            }
            
            app.voxelModelVerticies = (VoxelVertex*) realloc(app.voxelModelVerticies, sizeof(VoxelVertex) * app.voxelCount * VERTEX_PER_CUBE);
            app.voxelModelIndicies = (unsigned int*) realloc(app.voxelModelIndicies, sizeof(unsigned int) * INDICIES_PER_CUBE * app.voxelCount);

            Voxel* voxelLeaves[app.voxelCount];

            int index = 0;
            get_leaf_voxels(app.voxelHead, voxelLeaves, &index);
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

                    app.voxelModelIndicies[elementIndex] = element + 6;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 5;
                    elementIndex++;
                    app.voxelModelIndicies[elementIndex] = element + 4;      
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

        glBindFramebuffer(GL_FRAMEBUFFER, app.frameBuffer);
        {
            ColorRGBAf col = app.sceneBackColor;
            glClearColor(col.r, col.g, col.b, col.a);
        }  
        glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        glBindBuffer(GL_ARRAY_BUFFER, app.floorGridVbo);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7* sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE,7 * sizeof(float),(void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glUseProgram(app.gridShader);
        glUniformMatrix4fv(glGetUniformLocation(app.gridShader,"uProjMat"),1,GL_FALSE, (float*)app.projMat);
        glUniformMatrix4fv(glGetUniformLocation(app.gridShader,"uViewMat"),1,GL_FALSE, (float*)app.viewMat);
        glDrawArrays(GL_LINES,0, app.floorGridVertexCount);
        
        glBindBuffer(GL_ARRAY_BUFFER, app.voxelModelVertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.vertexModelIndexBuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7* sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glUseProgram(app.voxelShader);
        glUniformMatrix4fv(glGetUniformLocation(app.voxelShader,"uProjMat"),1,GL_FALSE, (float*)app.projMat);
        glUniformMatrix4fv(glGetUniformLocation(app.voxelShader,"uViewMat"),1,GL_FALSE, (float*)app.viewMat);
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