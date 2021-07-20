#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../linmath/linmath.h"


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

#ifndef NUKLEAR_H
#define NUKLEAR_H
#include "../nuklear/nuklear.h"
#endif
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
#include "../include/vector4.h"
#include "../include/app_viewport.h"
#include "../include/aabb.h"
#include "../include/ray.h"
#include "../include/prim_fact.h"
#include "../include/nuklear_gui.h"
#include "../include/app.h"
#include "../include/shader_resource.h"
#include "../include/voxel.h"
#include "../include/add_voxel_shape.h"
#include "../include/sphere.h"

enum app_cursor_focus{APP_CURSOR_FOCUS_GUI,APP_CURSOR_FOCUS_RENDER_VIEWPORT, APP_CURSOR_FOCUS_OUTSIDE_WINDOW};

typedef struct AppGlobals{
    float yScroll;
    float xScroll;
}AppGlobals;

void do_cursor_event(App* app){
    app->cursorButtonState[CURSOR_BUTTON_LEFT] = 
            cursor_button_next_state(app->cursorButtonState[CURSOR_BUTTON_LEFT], app->leftBtn == GLFW_PRESS
        );
    app->cursorButtonState[CURSOR_BUTTON_RIGHT] =
        cursor_button_next_state(app->cursorButtonState[CURSOR_BUTTON_RIGHT], app->rightBtn == GLFW_PRESS
    );
}

void do_update_inverse_matrix(App *app){
    mat4x4 projView;
    mat4x4_mul(projView, app->projMat, app->viewMat);
    mat4x4_invert(app->inverseMat, projView);
}

void do_update_cursor_ray(App *app){
    memcpy(app->cursor.ray.origin,app->orbitCameraPosition.vec, sizeof(vec3));
    Vector4 clipTarget = {{app->cursor.clipPosition.x, -app->cursor.clipPosition.y, -1.0f, 1.0f}};
    Vector4 worldTarget;
    mat4x4_mul_vec4(worldTarget.vec, app->inverseMat, clipTarget.vec);

    vec3_sub(worldTarget.xyz, worldTarget.xyz, app->cursor.ray.origin);
    vec3_norm(app->cursor.ray.direction, worldTarget.xyz);
}

void do_orbit_camera_event(App* app){
    switch (app->orbitCamera.state)
        {
        case ORBIT_CAMERA_STATE_IS_ORBITING:
            if(app->cursorButtonState[CURSOR_BUTTON_RIGHT] == CURSOR_BUTTON_STATE_UP){
                app->orbitCamera.state = ORBIT_CAMERA_STATE_NOT_ORBITING;
            }
            
            app->orbitCamera.yaw += 0.016 * app->cursor.delta.x;
            app->orbitCamera.pitch += 0.016 * app->cursor.delta.y;
            break;
        case ORBIT_CAMERA_STATE_NOT_ORBITING:
            if(app->cursorButtonState[CURSOR_BUTTON_RIGHT] == CURSOR_BUTTON_STATE_DOWN){
                app->orbitCamera.state = ORBIT_CAMERA_STATE_IS_ORBITING;

            }
            break;
        }
}


void do_update_cursor_with_glfw(App *app, GLFWwindow* window){
    double x, y;
    glfwGetCursorPos(window, &x ,&y);
    app->cursor.delta.x = x - app->cursor.position.x;
    app->cursor.delta.y = y - app->cursor.position.y;
    app->cursor.position.x = x;
    app->cursor.position.y = y;
}

void  do_set_hitpoint_voxel_model(App* app, Vector3 *hitpoint){
    switch (app->voxelManipulationMode)
    {
    case VOXEL_EDIT_ADD_VOXEL:
        switch (app->cursor.sideHit)
        {
        case BOX_SIDE_LEFT:
            hitpoint->x = round(hitpoint->x) - 1;
            break;
        case BOX_SIDE_RIGHT:
            hitpoint->x = round(hitpoint->x);
            break;
        case BOX_SIDE_BOTTOM:
            hitpoint->y = round(hitpoint->y) - 1;
            break;
        case BOX_SIDE_TOP:
            hitpoint->y = round(hitpoint->y);
            break;
        case BOX_SIDE_FRONT:
            hitpoint->z = round(hitpoint->z) - 1;
            break;
        case BOX_SIDE_BACK:
            hitpoint->z = round(hitpoint->z);
            break;
        }
        break;
    
    case VOXEL_EDIT_DELETE_VOXEL:
        switch (app->cursor.sideHit)
        {
        case BOX_SIDE_LEFT:
            hitpoint->x = round(hitpoint->x);
            break;
        case BOX_SIDE_RIGHT:
            hitpoint->x = round(hitpoint->x)  -1;
            break;
        case BOX_SIDE_BOTTOM:
            hitpoint->y = round(hitpoint->y);
            break;
        case BOX_SIDE_TOP:
            hitpoint->y = round(hitpoint->y) -1;
            break;
        case BOX_SIDE_FRONT:
            hitpoint->z = round(hitpoint->z);
            break;
        case BOX_SIDE_BACK:
            hitpoint->z = round(hitpoint->z) - 1 ;
            break;
        }
        break;
    }
}

void do_update_frame_buffer(App *app){

    glBindTexture(GL_TEXTURE_2D, app->frameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, app->renderViewport.width, app->renderViewport.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, app->frameDepthTexture); 
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, app->renderViewport.width,app->renderViewport.height, 
        0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
    );

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
    struct nk_glfw glfw = {0};
    struct nk_context *ctx;
    GLFWwindow *window;
    App app;

    float *voxelHeadVertexArray = (float*) malloc(sizeof(float) * 8 * 3);
    unsigned int *voxelHeadElementArray = (unsigned int*) malloc(sizeof(unsigned int) * 36);
    GLuint voxelHeadVertexBuffer, voxelHeadElementBuffer;
    
    float *rayhitModelVertexArray = (float*) malloc(sizeof(float) * 8 * 3);
    float *rayhitModelElementArray = (unsigned int*) malloc(sizeof(unsigned int) * 36);
    GLuint rayhitVertexBuffer, rayhitElementBuffer;

    float* voxelModelVertexArray = (float*) malloc(sizeof(float));
    unsigned int* voxelModelElementArray = (unsigned int*) malloc(sizeof(unsigned int));
    GLuint voxelModelVertexBuffer, voxelModelElementBuffer;

    float* sphereModelVertexArray = (float*) malloc(sizeof(float));
    unsigned int* sphereModelElementArray = (unsigned int*) malloc(sizeof(unsigned int));
    GLuint sphereModelVertexBuffer, sphereModelElementBuffer;

    init_globals(&appGlobals);
    app.appMode = APP_MODE_DEFAULT;
    if(argv[1] != NULL){
        if (strcmp(argv[1], "debug") == 0)
        {
            app.appMode = APP_MODE_DEBUG;
        }
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
        init_orbit_camera(&app.orbitCamera, pos, M_PI/4, -3*M_PI/4,M_PI/2,1.0,10000.0,app.renderViewport.ratio,10.0);
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
        app.cursor.position.x = (float)x;
        app.cursor.position.y = (float)y;
        app.cursor.delta.x = 0;
        app.cursor.delta.y = 0;
        
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
    if(load_shader_program(&app.shaderId[SHADER_ID_GRID_SHADER],"res/shaders/grid_vertex.glsl", "res/shaders/grid_fragment.glsl")){
        printf("failed to load shader\n");
        return 1;
    }

    if(load_shader_program(&app.shaderId[SHADER_ID_VOXEL_SHADER],"res/shaders/voxel_vertex.glsl","res/shaders/voxel_fragment.glsl")){
        printf("failed to load nessessary shader\n");
        return 1;
    }

    if(load_shader_program(&app.shaderId[SHADER_ID_PRIM_SHADER],"res/shaders/prim_vertex.glsl","res/shaders/prim_fragment.glsl")){
        printf("failed to load nessessary shader\n");
        return 1;
    }
   
    glGenBuffers(1, &app.floorGridVbo);
    glBindBuffer(GL_ARRAY_BUFFER, app.floorGridVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GridVertex) * app.floorGridVertexCount, app.floorGridModel, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Generate Voxel model vertex*/
    glGenBuffers(1, &voxelModelVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, voxelModelVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &voxelModelElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelModelElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    /* Generate Voxel head  vertex*/
    glGenBuffers(1, &voxelHeadVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, voxelHeadVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &voxelHeadElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelHeadElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /*Generate Ray Hit vertex*/
    glGenBuffers(1, &rayhitVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, rayhitVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &rayhitElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rayhitElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /*Generate Sphere Model Vertex*/
    glGenBuffers(1, &sphereModelVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphereModelVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &sphereModelElementBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphereModelElementBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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
    {
        ColorRGBAf color = {{0.5, 0.5, 0.5, 1.0}};
        memcpy(app.colorData[COLOR_DATA_BACKGROUND_COLOR].color, &color, sizeof(ColorRGBAf));
    }
    app.orbitCamera.state = ORBIT_CAMERA_STATE_NOT_ORBITING;
    app.propertySelectorWidth = 64;
    app.propertyWidth = 128;
    app.appCursorFocus = APP_CURSOR_FOCUS_GUI;


    app.voxelManipulationMode = VOXEL_EDIT_ADD_VOXEL;
    app.voxelHead = NULL;
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
        
        init_app_viewport(
            &app.renderViewport,
            app.propertyWidth + app.propertySelectorWidth, 
            app.menuBarHeight,
            app.renderPanelWidth,
            app.windowViewport.height - app.menuBarHeight
        );
        app.fileMenuSize[0] = 64;
        app.fileMenuSize[1] = 64;

        {
            float clipX, clipY;  
            do_update_cursor_with_glfw(&app, window);
            get_clip_position_from_cursor(&clipX, &clipY, app.cursor.position.x, app.cursor.position.y,
                app.renderViewport.x, app.renderViewport.y, 
                app.renderViewport.width, app.renderViewport.height
            );
            app.cursor.clipPosition.x = clipX;
            app.cursor.clipPosition.y = clipY;
        }
        
     
        app.leftBtn = glfwGetMouseButton(window, 0);
        app.rightBtn = glfwGetMouseButton(window, 1);        
        do_cursor_event(&app);
        
        app.orbitCamera.state = ORBIT_CAMERA_STATE_IS_ORBITING *
            (app.cursorButtonState[CURSOR_BUTTON_RIGHT] == CURSOR_BUTTON_STATE_DOWN) +
            ORBIT_CAMERA_STATE_NOT_ORBITING * 
            (app.cursorButtonState[CURSOR_BUTTON_RIGHT] == CURSOR_BUTTON_STATE_UP);

        do_orbit_camera_event(&app);
        
        app.orbitCamera.distance += appGlobals.yScroll;
        app.orbitCamera.aspectRat = app.renderViewport.ratio;

        get_projection_matrix_orbit_camera(app.orbitCamera, app.projMat);
        get_view_matrix_orbit_camera(app.orbitCamera, app.viewMat);
        do_update_inverse_matrix(&app);
        get_position_orbit_camera(app.orbitCamera, app.orbitCameraPosition.vec);
        do_update_cursor_ray(&app);

        

        app_globals_zero_scrolls(&appGlobals);
        glfwPollEvents();  

        /*Raycasting stuff*/
        {
            int hit = 0, side, entityType = ENTITY_TYPE_NONE;
            float hitDistance = 1000.0f;
            /*First test Ray vs floor*/
            app.cursor.hit = 0;
            if(app.cursor.ray.dy < 0){
                float d = (-app.cursor.ray.y) / app.cursor.ray.dy;
                if ((d >= 0) && (d < hitDistance) ){
                   hit = 1;
                   hitDistance = d;
                   entityType = ENTITY_TYPE_FLOOR;
                }
            }
            /*Not test ray vs voxel model*/ 
            voxel_vs_ray(app.voxelHead, app.cursor.ray, &entityType, &hit, &side, &hitDistance);
            if(hit){
                app.cursor.hit = 1;
                app.entityHit = entityType;
                app.cursor.sideHit = side;
                vec3_scale(app.cursor.hitPosition.vec, app.cursor.ray.direction, hitDistance);
                vec3_add(app.cursor.hitPosition.vec, app.cursor.hitPosition.vec, app.cursor.ray.origin);
            }
            
        }

        if(app.appMode == APP_MODE_DEBUG){
            /*Generear the Ray hit model*/
            vec3 halfExtents = {0.1, 0.1, 0.1};
            generate_cube_from_centre(app.cursor.hitPosition.vec, halfExtents, rayhitModelVertexArray, rayhitModelElementArray);
            glBindBuffer(GL_ARRAY_BUFFER, rayhitVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER,  24 * sizeof(float) , rayhitModelVertexArray, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rayhitElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int) , rayhitModelElementArray, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            /*generate sphere model*/
            /*
            generate_sphere_model(app.sphereBrush, 6, 6, sphereModelVertexArray, sphereModelElementArray);
            glBindBuffer(GL_ARRAY_BUFFER, sphereModelVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float), sphereModelVertexArray, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereModelElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int), sphereModelElementArray, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
        }


        if(app.cursorButtonState[CURSOR_BUTTON_LEFT] == CURSOR_BUTTON_STATE_PRESSED)
        {
            Vector3 hp = app.cursor.hitPosition;

            if(app.entityHit == ENTITY_TYPE_VOXEL_MODEL){ 
                do_set_hitpoint_voxel_model(&app, &hp);               
            }
            printf("hp calculation %f, %f, %f\n", hp.x, hp.y, hp.z);
            if((hp.x >= 0) && (hp.y >= 0) && (hp.z >= 0)){
                unsigned int vx,vy,vz;
                vx = (unsigned int) hp.x;
                vy = (unsigned int) hp.y;
                vz = (unsigned int) hp.z;
                printf("voxel mode %d\n",app.voxelManipulationMode);

                switch (app.voxelManipulationMode)
                {
                case VOXEL_EDIT_ADD_VOXEL:
                    printf("add voxel \n");

                    printf("shapae is %d\n",app.voxelManipulationShape);
                    switch (app.voxelManipulationShape)
                    {
                    case VOXEL_SHAPE_POINT:
                        voxel_add_voxel(&app.voxelHead, vx, vy, vz);
                        break;
                    case VOXEL_SHAPE_SPHERE:
                        {
                        printf("add voxel from sphere \n");
                        Sphere sphere;
                        sphere.r = 16.0f;
                        sphere.x = (float)(vx + 0.5f);
                        sphere.y = (float)(vy + 0.5f);
                        sphere.z = (float)(vz + 0.5f);
                        voxel_add_voxel_from_sphere(&app.voxelHead,sphere);
                        }
                        break;
                    case VOXEL_SHAPE_AABB:
                        //voxel_add_voxel_from_aabb(&app.voxelHead, vx, vy, vz);
                        break;
                    }
                    printf("voxel added to location %u, %u, %u \n", vx, vy, vz);
                    printf("voxel head size %u\n", app.voxelHead->size);
                    printf("voxel count %d\n", voxel_count_voxels(app.voxelHead));
                    break;
                
                case VOXEL_EDIT_DELETE_VOXEL:
                    switch (app.voxelManipulationShape)
                    {
                    case VOXEL_SHAPE_POINT:
                        voxel_delete_voxel(&app.voxelHead, vx, vy, vz);
                        break;
                    case VOXEL_SHAPE_SPHERE:
                        {
                        Sphere sphere;
                        sphere.x = (float)(vx + 0.5f);
                        sphere.y = (float)(vy + 0.5f);
                        sphere.z = (float)(vz + 0.5f);
                        sphere.r = 20;
                        }
                        //voxel_delete_voxel_from_sphere(&app.voxelHead, vx, vy, vz);
                        break;
                    case VOXEL_SHAPE_AABB:
                        //voxel_delete_voxel_from_aabb(&app.voxelHead, vx, vy, vz);

                        break;
                    }
                    break;
                }
            }

            if(app.appMode == APP_MODE_DEBUG){
                int voxelCount = voxel_count_voxels(app.voxelHead);
                voxelModelVertexArray = realloc(voxelModelVertexArray, voxelCount * 24 * sizeof(float));
                voxelModelElementArray = realloc(voxelModelElementArray, voxelCount * 36 * sizeof(unsigned int)) ;
                voxel_get_verticies_and_indicies(app.voxelHead, voxelModelVertexArray, voxelModelElementArray);
                if(voxelCount != 0){
                    /*set model buffer data*/
                    glBindBuffer(GL_ARRAY_BUFFER, voxelModelVertexBuffer);
                    glBufferData(GL_ARRAY_BUFFER, voxelCount * 24 * sizeof(float) , voxelModelVertexArray, GL_DYNAMIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelModelElementBuffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, voxelCount * 36 * sizeof(unsigned int) , voxelModelElementArray, GL_DYNAMIC_DRAW);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                    /*set model head data*/
                    voxel_generate_cube(app.voxelHead,0,voxelHeadVertexArray, voxelHeadElementArray);
                    glBindBuffer(GL_ARRAY_BUFFER, voxelHeadVertexBuffer);
                    glBufferData(GL_ARRAY_BUFFER,  24 * sizeof(float) , voxelHeadVertexArray, GL_DYNAMIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelHeadElementBuffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int) , voxelHeadElementArray, GL_DYNAMIC_DRAW);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                }
                
            }
        }
        do_update_frame_buffer(&app);
        
        glViewport(0,0, app.renderViewport.width, app.renderViewport.height);
       
        glBindFramebuffer(GL_FRAMEBUFFER, app.frameBuffer);
        
        ColorRGBAf col = app.colorData[COLOR_DATA_BACKGROUND_COLOR];
        glClearColor(col.r,col.b,col.g,col.a);
          
         
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        /* App Deug*/
        if(app.appMode == APP_MODE_DEBUG){
            int voxelCount = voxel_count_voxels(app.voxelHead);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUseProgram(app.shaderId[SHADER_ID_PRIM_SHADER]);
            glUniformMatrix4fv(glGetUniformLocation(app.shaderId[SHADER_ID_PRIM_SHADER],"uProjMat"),1,GL_FALSE, (float*)app.projMat);
            glUniformMatrix4fv(glGetUniformLocation(app.shaderId[SHADER_ID_PRIM_SHADER],"uViewMat"),1,GL_FALSE, (float*)app.viewMat);
            glUniform3f(glGetUniformLocation(app.shaderId[SHADER_ID_PRIM_SHADER],"uColor"),0.0,1.0,0.0);
            if(voxelCount != 0){
                glBindBuffer(GL_ARRAY_BUFFER, voxelModelVertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelModelElementBuffer);
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3* sizeof(float),(void*)0);
                glEnableVertexAttribArray(0);
                glDrawElements(GL_TRIANGLES, voxelCount * 36, GL_UNSIGNED_INT, 0);

                glUniform3f(glGetUniformLocation(app.shaderId[SHADER_ID_PRIM_SHADER],"uColor"),1.0,0.0,1.0);
                glBindBuffer(GL_ARRAY_BUFFER, voxelHeadVertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelHeadElementBuffer);
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3* sizeof(float),(void*)0);
                glEnableVertexAttribArray(0);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
            glUniform3f(glGetUniformLocation(app.shaderId[SHADER_ID_PRIM_SHADER],"uColor"),0.0,1.0,1.0);
            glBindBuffer(GL_ARRAY_BUFFER, rayhitVertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rayhitElementBuffer);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3* sizeof(float),(void*)0);
            glEnableVertexAttribArray(0);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, app.floorGridVbo);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7* sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE,7 * sizeof(float),(void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glUseProgram(app.shaderId[SHADER_ID_GRID_SHADER]);
        glUniformMatrix4fv(glGetUniformLocation(app.shaderId[SHADER_ID_GRID_SHADER],"uProjMat"),1,GL_FALSE, (float*)app.projMat);
        glUniformMatrix4fv(glGetUniformLocation(app.shaderId[SHADER_ID_GRID_SHADER],"uViewMat"),1,GL_FALSE, (float*)app.viewMat);
        glDrawArrays(GL_LINES,0, app.floorGridVertexCount);
       
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        
        nk_glfw3_new_frame(&glfw);
        do_show_main_window(ctx, &app);
        if(app.appMode == APP_MODE_DEBUG){    
            do_show_debug_window(ctx, app);
        }
        
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        
        glfwSwapBuffers(window);

    }     
    nk_glfw3_shutdown(&glfw);
    return 0;

}