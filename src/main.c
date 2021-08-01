
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
#include "../include/voxel_editor_shape_id.h"
#include "../include/voxel_edit_mode_id.h"
#include "../include/voxel_edit_dynamic_id.h"
#include "../include/property_menu_id.h"
#include "../include/app_nk_gui.h"
#include "../include/camera_mode.h"
#include "../include/tool_id.h"
#include "../include/selection_tool_shape.h"
#include "../include/selection_tool_state.h"
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

    app_update_window_glfw(&app, window);
    app.menuBarHeight = 32;
    app.propertySelectorWidth = 32;
    app.propertyWidth = 128;
    app.actionBarHeight = 32;

    {

        Camera camera = {CAMERA_MODE_PERSPECTIVE, 16, 16, 
            0, 0, 10,
            M_PI_2, 1.0, 1000.0, app.windowRatio,
            0,0,0
        };
        memcpy(&app.camera, &camera, sizeof(Camera));
    }
    
    {
        GridXZ floorGrid = {0.0f,0.0f,1.0f,1.0f,32,32};
        memcpy(&app.floorGrid, &floorGrid, sizeof(GridXZ));
    }

    app.voxelCount = 0;
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
    
    printf("cursor button state \n");

    app.shaderProgramId[SHADER_PROGRAM_ID_COLOR_ONLY] = app_generate_shader_program_from_file(
        &app,
        (const char*)"res/shaders/color_only.vert",
        (const char*)"res/shaders/color_only.frag"
    );
    printf("generated shader \n");
    //Initialize Buffers    
    glGenBuffers(VERTEX_BUFFER_ID_COUNT, app.vertexBufferId);
    glGenBuffers(ELEMENT_BUFFER_ID_COUNT, app.elementBufferId);
    printf("genetated buffers \n");


    //Initialize floor Grid
    glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_ID_FLOOR_GRID]);
    app_generate_floor_grid_vertex_array(&app);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    printf("floor grid \n");

    //initialize vertex model
    glGenBuffers(1, &app.voxelModelVertexBuffer);
    glGenBuffers(1, &app.voxelModelElementBuffer);
    app.voxelModelVertexArray = (PosColor32Vertex*) malloc(sizeof(PosColor32Vertex));
    printf("generated model \n");

    //initialize voxel head buffer
    glGenBuffers(1, &app.voxelHeadVertexBuffer);
    glGenBuffers(1, &app.voxelHeadElementBuffer);

    //Generate Frame buffer texture
    glGenFramebuffers(FRAME_BUFFER_ID_COUNT, app.frameBufferId);
    glGenTextures(TEXTURE_ID_COUNT, app.textureId);
    app_update_app_view_frame(&app);
    printf("generated head \n");

    glfwSetScrollCallback(window, scroll_callback);
    app.propertyMenuId = PROPERTY_MENU_ID_VOXEL_MENU;
    app.cursorPanelFocus = CURSOR_PANEL_FOCUS_GUI;
    app.voxelEditorShapeId = VOXEL_EDITOR_SHAPE_ID_POINT;
    app.voxelEditModeId = VOXEL_EDIT_MODE_ID_ADD;
    app.voxelEditorDynamicId = VOXEL_EDIT_DYNAMIC_PLOP;
    
    app.actionBarText = (char*) calloc(128,sizeof(char));
    app.actionBarLength = 128; 

    app.activeTool = TOOL_ID_VOXEL_MANIPULATOR; 
    app.activeSecondaryTool = TOOL_ID_BOX_SCALE_TOOL;

    time = glfwGetTime();
    while (!glfwWindowShouldClose(window)){
        app_update_window_glfw(&app, window);
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
        //update camera distance
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
            {
                vec3 origin;
                aabb_get_centre(app.selectionBox, origin);
                app_update_move_tool_pulley(&app, origin);
            }

            {
                Aabb box = app.selectionBox;
                app_update_box_scale_tool_pulley(&app, box);

            }
     
            switch (app.activeTool)
            {
                case TOOL_ID_VOXEL_MANIPULATOR:
                    app_use_voxel_manipulator(&app, deltaTime);
                    break;
                case TOOL_ID_SELECTION_TOOL:
                    app_use_selection_tool(&app);
                    if(glfwGetKey(window, GLFW_KEY_ENTER)){
                        app.selectionToolState = SELECTION_TOOL_STATE_CONFIRMED;
                          

                    }
                    break;
                case TOOL_ID_MOVE_TOOL:
                    break;
                case TOOL_ID_FLIP_TOOL:
                    break;

            }

            if(app.selectionToolState == SELECTION_TOOL_STATE_CONFIRMED){
                switch (app.voxelEditModeId)
                {
                case VOXEL_EDIT_MODE_ID_ADD:
                    voxel_model_add_voxel_from_aabb(&app.voxelModel, app.selectionBox);

                    break;
                
                case VOXEL_EDIT_MODE_ID_DELETE:
                    voxel_model_delete_voxel_from_aabb(&app.voxelModel, app.selectionBox);

                    break;
                }
                app.voxelCount = voxel_model_count_voxels(&app.voxelModel);
                if(app.voxelCount){
                    app_update_voxel_model(&app);
                }  

            }
            
            if(app.selectionToolState == SELECTION_TOOL_STATE_CONFIRMED){
                app.selectionToolState = SELECTION_TOOL_STATE_PICK_ORIGIN;
            }

            if(app.cursorRayDidHit){
                app_update_ray_hit_model(&app);
            }
           
            if(app.voxelModel.head){
                app_update_voxel_head_model(&app);   
            }
            
        }
        if(app.activeTool != TOOL_ID_SELECTION_TOOL){
            app_reset_selection_tool(&app);
        }
        app.selectionShape = SELECTION_TOOL_SHAPE_CUBE;

        if(app.selectionShape == SELECTION_TOOL_SHAPE_CUBE &&
            (app.selectionToolState != SELECTION_TOOL_STATE_PICK_ORIGIN)){
            {                
                ColorRgbaF color = {0.0,1.0,1.0,1.0};
                app_update_box_buffer(app.selectionBox, &app.vertexBufferId[VERTEX_BUFFER_SELECTION_BOX], 
                    &app.elementBufferId[ELEMENT_BUFFER_SELECTION_BOX], color
                );
            }
        }
        app_move_tool_update_model(&app);
        app_box_scale_tool_update_model(&app);
        


        //Draw Section
        glViewport(0,0, app.windowWidth, app.windowHeight);
        glClearColor(app.backgroundColor.r, app.backgroundColor.g, app.backgroundColor.b, app.backgroundColor.a);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, app.frameBufferId[FRAME_BUFFER_ID_APP_VIEW]);
        glUseProgram(app.shaderProgramId[SHADER_PROGRAM_ID_COLOR_ONLY]);
        
        glUniformMatrix4fv(
            glGetUniformLocation(app.shaderProgramId[SHADER_PROGRAM_ID_COLOR_ONLY],"uProjMat"),
            1, GL_FALSE, (float*)app.projectionMatrix
        );
        glUniformMatrix4fv(
            glGetUniformLocation(app.shaderProgramId[SHADER_PROGRAM_ID_COLOR_ONLY],"uViewMat"),
            1, GL_FALSE, (float*)app.viewMatrix
        );
        glViewport(0,0, app.appViewRect.w, app.appViewRect.h);
        glClearColor(app.backgroundColor.r, app.backgroundColor.g, app.backgroundColor.b, app.backgroundColor.a);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(app.voxelModel.head){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBindBuffer(GL_ARRAY_BUFFER, app.voxelHeadVertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.voxelHeadElementBuffer);
            app_use_pos_color_32_vertex();
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if(app.voxelCount){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBindBuffer(GL_ARRAY_BUFFER, app.voxelModelVertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.voxelModelElementBuffer);
            app_use_pos_color_32_vertex();
            glDrawElements(GL_TRIANGLES, app.voxelCount * 36, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if(app.cursorRayDidHit && app.cursorPanelFocus == CURSOR_PANEL_FOCUS_APP_VIEW){
            glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_ID_RAY_HIT]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_ID_RAY_HIT]);
            app_use_pos_color_32_vertex();
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); 
        }

        if(app.floorGridVertexCount){
            glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_ID_FLOOR_GRID]);
            app_use_pos_color_32_vertex();
            glDrawArrays(GL_LINES, 0, app.floorGridVertexCount);
        }

        if((app.activeTool == TOOL_ID_SELECTION_TOOL) && 
            (app.selectionToolState != SELECTION_TOOL_STATE_PICK_ORIGIN)){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_SELECTION_BOX]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_SELECTION_BOX]);
            app_use_pos_color_32_vertex();
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        }
        
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_X_MOVE_PULLEY]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_X_MOVE_PULLEY]);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_Y_MOVE_PULLEY]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_Y_MOVE_PULLEY]);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_Z_MOVE_PULLEY]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_Z_MOVE_PULLEY]);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_NX_BOX_SCALE_PULLEY]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_NX_BOX_SCALE_PULLEY]);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

          
        glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_PX_BOX_SCALE_PULLEY]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_PX_BOX_SCALE_PULLEY]);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_NY_BOX_SCALE_PULLEY]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_NY_BOX_SCALE_PULLEY]);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

          
        glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_PY_BOX_SCALE_PULLEY]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_PY_BOX_SCALE_PULLEY]);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_NZ_BOX_SCALE_PULLEY]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_NZ_BOX_SCALE_PULLEY]);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

          
        glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_PZ_BOX_SCALE_PULLEY]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.elementBufferId[ELEMENT_BUFFER_PZ_BOX_SCALE_PULLEY]);
        app_use_pos_color_32_vertex();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);
        
        
        nk_glfw3_new_frame(&glfw);  
        app_nk_gui_main_window(&app, ctx);
        if(nk_begin(ctx, "Toolbar", nk_rect(0,0,64, 256),  
            NK_WINDOW_MINIMIZABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MOVABLE)){
                nk_layout_row_static(ctx, 48,48,1);
                int pred = (app.activeTool == TOOL_ID_SELECTION_TOOL) ? nk_true : nk_false;
                if(nk_select_label(ctx,"select", NK_TEXT_LEFT, pred)){
                    app.activeTool = TOOL_ID_SELECTION_TOOL;
                }
                pred = (app.activeTool == TOOL_ID_VOXEL_MANIPULATOR) ? nk_true : nk_false;
                if(nk_select_label(ctx,"voxeledit", NK_TEXT_LEFT, pred)){
                    app.activeTool = TOOL_ID_VOXEL_MANIPULATOR;
                }
                pred = (app.activeTool == TOOL_ID_MOVE_TOOL) ? nk_true : nk_false;
                if(!pred){
                    pred = (app.activeTool == TOOL_ID_SELECTION_TOOL && app.activeSecondaryTool == TOOL_ID_MOVE_TOOL) ?
                    nk_true : nk_false;
                }
                if(nk_select_label(ctx,"move tool", NK_TEXT_LEFT, pred)){
                    if(app.activeTool == TOOL_ID_SELECTION_TOOL){
                        app.activeSecondaryTool = TOOL_ID_MOVE_TOOL;
                    }
                }
                
                pred = (app.activeTool == TOOL_ID_BOX_SCALE_TOOL) ? nk_true : nk_false;
                 if(!pred){
                    pred = (app.activeTool == TOOL_ID_SELECTION_TOOL && app.activeSecondaryTool == TOOL_ID_BOX_SCALE_TOOL) ?
                    nk_true : nk_false;
                }
                if(nk_select_label(ctx,"scale tool", NK_TEXT_LEFT, pred)){
                    if(app.activeTool = TOOL_ID_SELECTION_TOOL){
                        app.activeSecondaryTool = TOOL_ID_BOX_SCALE_TOOL;
                    }
                }

        }
        nk_end(ctx);

        if(nk_begin(ctx, "Debug Window", nk_rect(0,0, 120, 120), 
            NK_WINDOW_MINIMIZABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MOVABLE)){
            char str[20 *4];
            nk_layout_row_static(ctx, 32, 96, 3);
        
            nk_label(ctx, "cursor_pos", NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f",app.cursor.x);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f",app.cursor.y);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            
            nk_label(ctx, "cursor_clip", NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursor.clipX);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursor.clipY);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            nk_layout_row_static(ctx, 32, 96,4);

            nk_label(ctx, "cam_pos", NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cameraPosition[0]);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cameraPosition[1]);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cameraPosition[2]);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            nk_layout_row_static(ctx, 32, 96,3);
            nk_label(ctx, "cam_y_p", NK_TEXT_ALIGN_LEFT);
             sprintf(str, "%f", app.camera.yaw);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.camera.pitch);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            nk_layout_row_static(ctx, 32, 96, 4);
            nk_label(ctx, "ray_o", NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursorRay.x);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursorRay.y);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursorRay.z);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            nk_label(ctx, "ray_d", NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursorRay.dx);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursorRay.dy);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursorRay.dz);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            nk_label(ctx, "ray_hp", NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursorRayHitPoint[0]);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursorRayHitPoint[1]);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            sprintf(str, "%f", app.cursorRayHitPoint[2]);
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
            nk_layout_row_static(ctx, 32, 128*3,1);
            
            nk_label(ctx, "view_mat", NK_TEXT_ALIGN_LEFT);
            sprintf(str, "[%f, %f, %f, %f]\n", 
                app.viewMatrix[0][0], app.viewMatrix[0][1], app.viewMatrix[0][2], app.viewMatrix[0][3]
            );
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            sprintf(str, "[%f, %f, %f, %f]\n", 
                app.viewMatrix[1][0], app.viewMatrix[1][1], app.viewMatrix[1][2], app.viewMatrix[1][3]
            );
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            sprintf(str, "[%f, %f, %f, %f]\n", 
                app.viewMatrix[2][0], app.viewMatrix[2][1], app.viewMatrix[2][2], app.viewMatrix[2][3]
            );
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            sprintf(str, "[%f, %f, %f, %f]\n", 
                app.viewMatrix[3][0], app.viewMatrix[3][1], app.viewMatrix[3][2], app.viewMatrix[3][3]
            );
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            nk_label(ctx, "proj_mat", NK_TEXT_ALIGN_LEFT);
            sprintf(str, "[%f, %f, %f, %f]\n", 
                app.projectionMatrix[0][0], app.projectionMatrix[0][1], app.projectionMatrix[0][2], app.projectionMatrix[0][3]
            );
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            sprintf(str, "[%f, %f, %f, %f]\n", 
                app.projectionMatrix[1][0], app.projectionMatrix[1][1], app.projectionMatrix[1][2], app.projectionMatrix[1][3]
            );
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            sprintf(str, "[%f, %f, %f, %f]\n", 
                app.projectionMatrix[2][0], app.projectionMatrix[2][1], app.projectionMatrix[2][2], app.projectionMatrix[2][3]
            );
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);

            sprintf(str, "[%f, %f, %f, %f]\n", 
                app.projectionMatrix[3][0], app.projectionMatrix[3][1], app.projectionMatrix[3][2], app.projectionMatrix[3][3]
            );
            nk_label(ctx,str, NK_TEXT_ALIGN_LEFT);
        }
        nk_end(ctx);
        
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
