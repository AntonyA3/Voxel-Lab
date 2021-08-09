
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

#ifndef NK_GUI_HEADER
#define NK_GUI_HEADER
#include "../nuklear/nuklear.h"
#endif
#include "../nuklear/nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define VERTEX_PER_CUBE 8
#define INDICIES_PER_CUBE 36
#define FPS 60
#include "../linmath/linmath.h"
#include "../include/app.h"
#include "../include/cursor.h"
#include "../include/cursor_button_id.h"
#include "../include/cursor_button_state.h"
#include "../include/grid_xz.h"
#include "../include/vertex_buffer_id.h"
#include "../include/element_buffer_id.h"
#include "../include/shader_program_id.h"
#include "../include/ray_hit_entity.h"
#include "../include/aabb_side.h"
#include "../include/cursor_panel_focus.h"
#include "../include/voxel_tree.h"
#include "../include/voxel_model.h"
#include "../include/voxel_editor_shape_id.h"
#include "../include/voxel_edit_mode_id.h"
#include "../include/voxel_edit_dynamic_id.h"
#include "../include/property_menu_id.h"
#include "../include/app_nk_gui.h"
#include "../include/camera_mode.h"
#include "../include/tool_id.h"
#include "../include/selection_tool_shape.h"
#include "../include/selection_tool_state.h"
#include "../include/voxel_mod.h"
typedef struct {
    float scrollY;
}GlobalVars;

GlobalVars globalVars;
void scroll_callback(GLFWwindow *window, double scrollX, double scrollY){
    globalVars.scrollY = (float)scrollY;

}


void file_get_text(const char *path, char **text){
    FILE *filepointer;
    long fileBytes = 0;
    filepointer = fopen(path, "r");
    if(!filepointer){
        printf("file does not exist \n");
    }
    fseek(filepointer, 0L, SEEK_END);
    fileBytes = ftell(filepointer);
    fseek(filepointer, 0L, SEEK_SET);
    *text = (char*)calloc(fileBytes, sizeof(char));

    if(*text == NULL){
        printf("not enough menmory\n");
    }
    fread((char*) *text, sizeof(char), fileBytes, filepointer);
    
}

int main(int argc, char const *argv[])
{
    struct nk_glfw glfw = {0};
    struct nk_context *ctx;
    float deltaTime = 1 / FPS;
    float time;
    GLFWwindow* window;
    App app;


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
    
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }  
    

    {
        ColorRgbaF color = {0.5, 0.5, 0.5, 1.0};
        memcpy(&app.backgroundColor, &color, sizeof(ColorRgbaF));
    }
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        Rect rect = {0,0, (float) width, (float)height};
        memcpy(&app.windowRect, &rect, sizeof(Rect));
        app.windowRatio = width /(float)height;
    }
    app.menuBarHeight = 32;
    app.propertySelectorWidth = 48;
    app.propertyWidth = 128;
    app.actionBarHeight = 32;

    {

        Camera camera = {CAMERA_MODE_PERSPECTIVE, 16, 16, 
            degToRad(-125),degToRad(-120), 16,
            M_PI_2, 1.0, 1000.0, app.windowRatio,
            0,0,0
        };
        memcpy(&app.camera, &camera, sizeof(Camera));
    }
    
    {
        GridXZ floorGrid = {0.0f,0.0f,1.0f,1.0f,32,32};
        memcpy(&app.floorGrid, &floorGrid, sizeof(GridXZ));
    }

    /*
    app.mainVoxels.voxelCount = 0;
    app.mainVoxels.offset[0] = 0;
    app.mainVoxels.offset[1] = 0;
    app.mainVoxels.offset[2] = 0;

    app.selectedVoxels.voxelCount = 0;
    app.selectedVoxels.offset[0] = 0;
    app.selectedVoxels.offset[1] = 0;
    app.selectedVoxels.offset[2] = 0;
    
    */
    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);    
    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);
        nk_glfw3_font_stash_end(&glfw);
    }       

    int *cursorButtonState = app.cursorButtonState;
    for(int i = 0; i < CURSOR_BUTTON_ID_COUNT; i++){
        cursorButtonState[i] = CURSOR_BUTTON_STATE_UP;
    }
    
    printf("begin flat shader loading\n");
    
    app.shaderProgramId[SHADER_PROGRAM_ID_FLAT] = app_generate_shader_program_from_file(
        &app,
        (const char*)"res/shaders/flat.vert",
        (const char*)"res/shaders/flat.frag"
    );
    printf("flat shader loaded\n");
    
    /*
    printf("begin phong shader loading\n");
    app.shaderProgramId[SHADER_PROGRAM_ID_PHONG] = app_generate_shader_program_from_file(
        &app,
        (const char*)"res/shaders/phong.vert",
        (const char*)"res/shaders/phong.frag"

    );
    
    printf("phong shader loaded\n");
    */
  
    glGenBuffers(VERTEX_BUFFER_ID_COUNT, app.vertexBufferId);
    glGenBuffers(ELEMENT_BUFFER_ID_COUNT, app.elementBufferId);


    glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_ID_FLOOR_GRID]);
    app_generate_floor_grid_vertex_array(&app);
    /*
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    {    
        //VoxelModel *voxelModels = {&app.mainVoxelsModel, &app.selectedVoxelsModel};

        for(int i = 0; i < 2; i++){
            glGenBuffers(1, &voxelModels[i].vertexBuffer);
            glGenBuffers(1, &voxelModels[i].elementBuffer);
            voxelModels[i].vertexArray = (float*) malloc(sizeof(float));
            voxelModels[i].elementArray = (unsigned int*) malloc(sizeof(unsigned int)); 
        }    
    }*/
    glGenBuffers(1, &app.voxelHeadVertexBuffer);
    glGenBuffers(1, &app.voxelHeadElementBuffer);

    //Generate Frame buffer texture
    glGenFramebuffers(FRAME_BUFFER_ID_COUNT, app.frameBufferId);
    glGenTextures(TEXTURE_ID_COUNT, app.textureId);
    //app_update_app_view_frame(&app);

    glfwSetScrollCallback(window, scroll_callback);
    app.propertyMenuId = PROPERTY_MENU_ID_VOXEL_MENU;
    app.cursorPanelFocus = CURSOR_PANEL_FOCUS_GUI;
    /*
    app.voxelEditor.shape = VOXEL_EDITOR_SHAPE_ID_POINT;
    app.voxelEditor.editMode = VOXEL_EDIT_MODE_ID_ADD;
    app.voxelEditor.dynamics = VOXEL_EDIT_DYNAMIC_PLOP;
    
    */
    app.actionBarText = (char*) calloc(128,sizeof(char));
    app.actionBarLength = 128; 

    app.activeTool = TOOL_ID_VOXEL_MANIPULATOR; 
    //app.selectionTool.subTool = TOOL_ID_BOX_SCALE_TOOL;

    time = glfwGetTime();
    while (!glfwWindowShouldClose(window)){        
        {   
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            app.windowRect.x = 0;
            app.windowRect.y = 0;
            app.windowRect.w = width;
            app.windowRect.h = height;
        }

        app_update_app_view(&app);
        app_update_app_view_frame(&app);

        app_update_cursor_glfw(&app, window);  
        {       
            int cursorInRect = rect_contains_point(app.appViewRect, app.cursor.x, app.cursor.y); 
            app.cursorPanelFocus = CURSOR_PANEL_FOCUS_GUI * !cursorInRect + CURSOR_PANEL_FOCUS_APP_VIEW * cursorInRect;
        }

        app.cursorButtonDown[CURSOR_BUTTON_ID_PRIMARY] = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
        app.cursorButtonDown[CURSOR_BUTTON_ID_SECONDARY] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;
        
        app.cursorButtonState[CURSOR_BUTTON_ID_PRIMARY] = app_get_cursor_button_next_state(
            app.cursorButtonState[CURSOR_BUTTON_ID_PRIMARY], app.cursorButtonDown[CURSOR_BUTTON_ID_PRIMARY]
        );

        app.cursorButtonState[CURSOR_BUTTON_ID_SECONDARY] = app_get_cursor_button_next_state(
            app.cursorButtonState[CURSOR_BUTTON_ID_SECONDARY], app.cursorButtonDown[CURSOR_BUTTON_ID_SECONDARY]
        );
        
        /*
        {
            int down = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS; 
            switch (app.enterButtonState)
            {
            case BUTTON_STATE_DOWN:
                if(down){
                    app.enterButtonState = BUTTON_STATE_DOWN;
                }else{
                    app.enterButtonState = BUTTON_STATE_RELEASED;
                }
                break;
            case BUTTON_STATE_PRESSED:
                if(down){
                    app.enterButtonState = BUTTON_STATE_DOWN;
                }else{
                    app.enterButtonState = BUTTON_STATE_RELEASED;
                }
                break;
            case BUTTON_STATE_RELEASED:
                if(down){
                    app.enterButtonState = BUTTON_STATE_PRESSED;
                }else{
                    app.enterButtonState = BUTTON_STATE_UP;
                }
                break;
            case BUTTON_STATE_UP:
                if(down){
                    app.enterButtonState = BUTTON_STATE_PRESSED;
                }else{
                    app.enterButtonState = BUTTON_STATE_UP;
                }
                break;
            }
        }
        */
       
        app.appViewCursorX = app.cursor.x - app.appViewRect.x;
        app.appViewCursorY = app.cursor.y - app.appViewRect.y;
        app.appViewCursorClipX = 2 * (app.appViewCursorX / app.appViewRect.w) - 1;
        app.appViewCursorClipY = -1 *((app.appViewCursorY / app.appViewRect.h) * 2 - 1);

        globalVars.scrollY = 0;
        glfwPollEvents();     

        app.camera.aspectRatio = app.appViewRatio;
        if(app.cursorButtonState[CURSOR_BUTTON_ID_SECONDARY] == CURSOR_BUTTON_STATE_DOWN){
            app.camera.yaw += deltaTime * app.cursor.dx;
            app.camera.pitch += deltaTime * app.cursor.dy;
        }
        app.camera.distance += globalVars.scrollY;
        camera_get_view_matrix(app.camera, app.viewMatrix);
        camera_get_projection_matrix(app.camera, app.projectionMatrix);
        {
            vec4 forward;
            mat4x4_row(forward, app.viewMatrix,2);
            app.cameraForward[0] = -forward[0];
            app.cameraForward[1] = -forward[1];
            app.cameraForward[2] = -forward[2];
        }
        
        
        if(app.cursorPanelFocus == CURSOR_PANEL_FOCUS_APP_VIEW){
            mat4x4_mul(app.projectionViewMatrix, app.projectionMatrix, app.viewMatrix);
            mat4x4_invert(app.invertedMatrix, app.projectionViewMatrix);
            camera_get_position(app.camera, app.cameraPosition);
            app_update_cursor_ray(&app);
            app_update_ray_hit_result(&app);
          
            if(app.cursorRayDidHit){
                app_update_ray_hit_model(&app);
                

                if(app.cursorButtonState[CURSOR_BUTTON_ID_PRIMARY] == CURSOR_BUTTON_STATE_PRESSED){

                    float box[6] = {app.cursorRayHitPoint[0], app.cursorRayHitPoint[1], 1, 1, 1};
                    voxel_tree_edit(&app.mainVoxels, SHAPE_AABB, box);
                    app.voxelCount = voxel_count_voxels(app.mainVoxels.head);
                    app_update_voxel_head_model(&app);
                }
            }  
        }
        
       
        /*
        if(app.activeTool != TOOL_ID_SELECTION_TOOL){
            app_reset_selection_tool(&app);
        }

        if(app.activeTool == TOOL_ID_SELECTION_TOOL)
        {                
            ColorRgbaF color = {0.0,1.0,1.0,1.0};
            app.selectedVoxels.voxelCount = voxel_tree_count_voxels(&app.selectedVoxels);
            app_update_box_buffer(app.selectionTool.selectionBox, &app.vertexBufferId[VERTEX_BUFFER_SELECTION_BOX], 
                &app.elementBufferId[ELEMENT_BUFFER_SELECTION_BOX], color
            );
        }
    
        app_move_tool_update_model(&app);
        app_box_scale_tool_update_model(&app);
        
        */

        //Draw Section
        glViewport(0,0, app.windowRect.w, app.windowRect.h);
        glClearColor(app.backgroundColor.r, app.backgroundColor.g, app.backgroundColor.b, app.backgroundColor.a);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindFramebuffer(GL_FRAMEBUFFER, app.frameBufferId[FRAME_BUFFER_ID_APP_VIEW]);
        
        
        glViewport(0,0, app.appViewRect.w, app.appViewRect.h);
        glClearColor(app.backgroundColor.r, app.backgroundColor.g, app.backgroundColor.b, app.backgroundColor.a);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(app.shaderProgramId[SHADER_PROGRAM_ID_FLAT]);
        
        glUniformMatrix4fv(
            glGetUniformLocation(app.shaderProgramId[SHADER_PROGRAM_ID_FLAT],"uProjMat"),
            1, GL_FALSE, (float*)app.projectionMatrix
        );
        glUniformMatrix4fv(
            glGetUniformLocation(app.shaderProgramId[SHADER_PROGRAM_ID_FLAT],"uViewMat"),
            1, GL_FALSE, (float*)app.viewMatrix
        );
        {
            mat4x4 idMat;
            mat4x4_identity(idMat);
            glUniformMatrix4fv(
                glGetUniformLocation(app.shaderProgramId[SHADER_PROGRAM_ID_FLAT],"uModelMat"),
                1, GL_FALSE, (float*)idMat
            );
        }
        if(app.floorGridVertexCount){
            glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_ID_FLOOR_GRID]);
            app_use_pos_color_32_vertex();
            glDrawArrays(GL_LINES, 0, app.floorGridVertexCount);
        }
        if(app.cursorRayDidHit && app.cursorPanelFocus == CURSOR_PANEL_FOCUS_APP_VIEW){
            glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_ID_RAY_HIT]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_ID_RAY_HIT]);
            app_use_pos_color_32_vertex();
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); 
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindBuffer(GL_ARRAY_BUFFER, app.voxelHeadVertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.voxelHeadElementBuffer);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); 
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

       
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);
        
        
        nk_glfw3_new_frame(&glfw);  

        
        app_nk_gui_main_window(&app, ctx);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(window);

        if(app.shouldQuit){
            glfwSetWindowShouldClose(window, app.shouldQuit);
        }
        
        deltaTime = (glfwGetTime() - time);
        time = glfwGetTime();
        
    }
    nk_glfw3_shutdown(&glfw);
    return 0;
}
