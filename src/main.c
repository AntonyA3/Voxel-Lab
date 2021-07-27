
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

#include "../nuklear/nuklear.h"
#include "../nuklear/nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define VERTEX_PER_CUBE 8
#define INDICIES_PER_CUBE 36

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
    
    app.backgroundColor.r = 0.5f;
    app.backgroundColor.g = 0.5f;
    app.backgroundColor.b = 0.5f;
    app.backgroundColor.a = 1.0f;

    app_update_window_glfw(&app, window);
    app.menuBarHeight = 32;
    app.propertySelectorWidth = 32;
    app.propertyWidth = 128;

    app.appViewWidth = app.windowWidth - app.propertyWidth - app.propertySelectorWidth;
    app.appViewHeight = app.windowHeight - 32;

    app.camera.far = 1000.0;
    app.camera.near = 1.0;
    app.camera.pitch = 0;
    app.camera.fov = M_PI_2;
    app.camera.aspectRatio = app.windowRatio;
    app.camera.distance = 10;
    app.camera.yaw = 0;
    app.camera.pitch = 0;
    
    app.floorGrid.x = 0.0f;
    app.floorGrid.z = 0.0f;
    app.floorGrid.sizeX = 1.0f;
    app.floorGrid.sizeZ = 1.0f;
    app.floorGrid.xCount = 32;
    app.floorGrid.zCount = 32;

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
    

    app.shaderProgramId[SHADER_PROGRAM_ID_COLOR_ONLY] = app_generate_shader_program_from_file(
        &app,
        (const char*)"res/shaders/color_only.vert",
        (const char*)"res/shaders/color_only.frag"
    );
    //Initialize Buffers    
    glGenBuffers(VERTEX_BUFFER_ID_COUNT, app.vertexBufferId);
    glGenBuffers(ELEMENT_BUFFER_ID_COUNT, app.elementBufferId);

    //Initialize floor Grid
    glBindBuffer(GL_ARRAY_BUFFER, app.vertexBufferId[VERTEX_BUFFER_ID_FLOOR_GRID]);
    app_generate_floor_grid_vertex_array(&app);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //initialize vertex model
    glGenBuffers(1, &app.voxelModelVertexBuffer);
    glGenBuffers(1, &app.voxelModelElementBuffer);
    app.voxelModelVertexArray = (PosColor32Vertex*) malloc(sizeof(PosColor32Vertex));

    //initialize voxel head buffer
    glGenBuffers(1, &app.voxelHeadVertexBuffer);
    glGenBuffers(1, &app.voxelHeadElementBuffer);


    //Generate Frame buffer texture
    
    glGenTextures(TEXTURE_ID_COUNT, app.textureId);

    glBindTexture(GL_TEXTURE_2D, app.textureId[TEXTURE_ID_APP_VIEW_TEXTURE]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //initialise Frame Buffer
    glGenFramebuffers(FRAME_BUFFER_ID_COUNT, app.frameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, app.frameBufferId[FRAME_BUFFER_ID_APP_VIEW]);
    /*glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,
        GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
    );*/
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, app.textureId[TEXTURE_ID_APP_VIEW_TEXTURE]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, app.textureId[TEXTURE_ID_APP_VIEW_TEXTURE], 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glfwSetScrollCallback(window, scroll_callback);

    while (!glfwWindowShouldClose(window)){
        app_update_window_glfw(&app, window);
        app_update_cursor_glfw(&app, window);
        app.cursorButtonDown[CURSOR_BUTTON_ID_PRIMARY] = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
        app.cursorButtonDown[CURSOR_BUTTON_ID_SECONDARY] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;
        
        app.cursorButtonState[CURSOR_BUTTON_ID_PRIMARY] = app_get_cursor_button_next_state(
            app.cursorButtonState[CURSOR_BUTTON_ID_PRIMARY], app.cursorButtonDown[CURSOR_BUTTON_ID_PRIMARY]
        );

        app.cursorButtonState[CURSOR_BUTTON_ID_SECONDARY] = app_get_cursor_button_next_state(
            app.cursorButtonState[CURSOR_BUTTON_ID_SECONDARY], app.cursorButtonDown[CURSOR_BUTTON_ID_SECONDARY]
        );

        globalVars.scrollY = 0;
        glfwPollEvents();     
        
        if(app.cursorButtonState[CURSOR_BUTTON_ID_SECONDARY] == CURSOR_BUTTON_STATE_DOWN){
            app.camera.yaw += 0.016 * app.cursor.dx;
            app.camera.pitch += 0.016 * app.cursor.dy;
        }
        //update camera distance
        app.camera.distance += globalVars.scrollY;
        camera_get_view_matrix(app.camera, app.viewMatrix);
        camera_get_projection_matrix(app.camera, app.projectionMatrix);
        mat4x4_mul(app.projectionViewMatrix, app.projectionMatrix, app.viewMatrix);
        mat4x4_invert(app.invertedMatrix, app.projectionViewMatrix);
        camera_get_position(app.camera, app.cameraPosition);
        app_update_cursor_ray(&app);
        {
            float distance = 1000;
            int hitEntity = RAY_HIT_ENTITY_NONE;
            int hitSide = AABB_SIDE_LEFT;
            app.cursorRayDidHit = 0;
        
            app_ray_vs_y0(&app, &app.cursorRayDidHit, app.cursorRay, &hitEntity, &distance);
            app_ray_vs_voxel_model(&app, &app.cursorRayDidHit, app.voxelModel, app.cursorRay, &hitEntity, &distance, &hitSide);
            
            app.cursorRayHitPoint[0] = app.cursorRay.x + app.cursorRay.dx * distance;
            app.cursorRayHitPoint[1] = app.cursorRay.y + app.cursorRay.dy * distance;
            app.cursorRayHitPoint[2] = app.cursorRay.z + app.cursorRay.dz * distance;   
        
            app.cursorHitEntity = hitEntity;
            app.cursorSideHit = hitSide;
            
        }
    
        if(app.cursorRayDidHit){
            app_update_ray_hit_model(&app);
            if(app.voxelModel.head){
                app_update_voxel_head_model(&app);   
            }

            if(app.cursorButtonState[CURSOR_BUTTON_ID_PRIMARY] == CURSOR_BUTTON_STATE_PRESSED){
                float targetX = -1.0;
                float targetY = -1.0;
                float targetZ = -1.0;
                switch (app.cursorHitEntity)
                {
                case RAY_HIT_ENTITY_Y0:
                    targetX = app.cursorRayHitPoint[0];
                    targetY = 0.0f;
                    targetZ = app.cursorRayHitPoint[2];
                    break;
                case RAY_HIT_ENTITY_VOXEL_MODEL:
                    targetX = app.cursorRayHitPoint[0];
                    targetY = app.cursorRayHitPoint[1];
                    targetZ = app.cursorRayHitPoint[2];
                    app_hit_target_from_voxel_model_hit(&app, app.cursorSideHit, &targetX, &targetY, &targetZ);
                    break;
                }
                
                if(targetX >= 0 && targetY >= 0 && targetZ >= 0){
                    unsigned int x = targetX;
                    unsigned int y = targetY;
                    unsigned int z = targetZ;
                    /*
                    Aabb aabb;
                    aabb.x = x;
                    aabb.y = y;
                    aabb.z = z;
                    aabb.w = 64;
                    aabb.h = 64;
                    aabb.d = 64;
                    voxel_model_add_voxel_from_aabb(&app.voxelModel, aabb);*/
                    //voxel_model_add_voxel(&app.voxelModel, x, y, z);

                    Sphere sphere;
                    sphere.x = x + 0.5;
                    sphere.y = y + 0.5;
                    sphere.z = z + 0.5;
                    sphere.r = 16.0f;
                    voxel_model_add_voxel_from_sphere(&app.voxelModel, sphere);
                    
                    app.voxelCount = voxel_model_count_voxels(&app.voxelModel);
                    printf("voxel count %d\n", app.voxelCount);
                    if(app.voxelCount){
                        app_update_voxel_model(&app);
                    }     
                }
            }         
        }

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
        
        glClearColor(app.backgroundColor.r, app.backgroundColor.g, app.backgroundColor.b, app.backgroundColor.a);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(app.voxelCount){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBindBuffer(GL_ARRAY_BUFFER, app.voxelModelVertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.voxelModelElementBuffer);
            app_use_pos_color_32_vertex();
            glDrawElements(GL_TRIANGLES, app.voxelCount * 36, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if(app.voxelModel.head){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBindBuffer(GL_ARRAY_BUFFER, app.voxelHeadVertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app.voxelHeadElementBuffer);
            app_use_pos_color_32_vertex();
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if(app.cursorRayDidHit){
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
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);
        
        nk_glfw3_new_frame(&glfw);
        
        if(nk_begin(ctx, "App Window", nk_rect(0,0,app.windowWidth, app.windowHeight), 
        NK_WINDOW_BACKGROUND | NK_WINDOW_NO_SCROLLBAR)){
            
            nk_menubar_begin(ctx);            
            struct nk_vec2 s = {64, 64};
            nk_layout_row_static(ctx, 32, 32, 3);
            if(nk_menu_begin_label(ctx, "File", NK_TEXT_LEFT, s)){
                nk_layout_row_dynamic(ctx, 25,1);
                nk_menu_item_label(ctx, "New", NK_TEXT_LEFT);
                nk_menu_end(ctx);
            }
            if(nk_menu_begin_label(ctx, "Edit", NK_TEXT_CENTERED, s)){
                nk_layout_row_dynamic(ctx, 25,1);
                nk_menu_item_label(ctx, "Undo", NK_TEXT_LEFT);
                nk_menu_item_label(ctx, "Redo", NK_TEXT_LEFT);

                nk_menu_end(ctx);
            }
            if(nk_menu_begin_label(ctx, "About", NK_TEXT_LEFT, s)){
                nk_menu_item_label(ctx, "Source Code", NK_TEXT_LEFT);
                nk_menu_end(ctx);
            }
            nk_menubar_end(ctx);


            nk_layout_row_begin(ctx, NK_STATIC,app.windowHeight - 32, 3);
            nk_layout_row_push(ctx, 64);
            if(nk_group_begin(ctx, "Selector Panel", NK_WINDOW_NO_SCROLLBAR)){
                nk_layout_row_static(ctx, 32, 32, 1);
                nk_select_label(ctx, "voxelEditProps", NK_TEXT_LEFT, nk_true);
                nk_select_label(ctx, "CameraProps", NK_TEXT_LEFT, nk_false);
                nk_group_end(ctx);   
            }

            nk_layout_row_push(ctx, 128);
            if(nk_group_begin(ctx, "Properties Panel", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_TITLE)){
                nk_layout_row_static(ctx, 32, 32, 1);
                nk_label(ctx, "Hello", NK_TEXT_LEFT);
                nk_group_end(ctx);   
            }

            nk_layout_row_push(ctx, app.windowWidth - 128 - 64);
            if(nk_group_begin(ctx, "View Panel", NK_WINDOW_NO_SCROLLBAR)){

                nk_layout_row_static(ctx, app.windowHeight - 32, app.windowWidth - 128 - 64, 1);
                struct nk_image image = nk_image_id(app.textureId[TEXTURE_ID_APP_VIEW_TEXTURE]);
                nk_image_color(ctx, image, nk_white);
                nk_group_end(ctx);   

            }
            nk_layout_row_end(ctx);
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

        
    }
    return 0;
}
