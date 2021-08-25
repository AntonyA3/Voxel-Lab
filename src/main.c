#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

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

#ifndef NK
#define NK
#include "../nuklear/nuklear.h"
#include "../nuklear/nuklear_glfw_gl3.h"
#endif

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024


#define FPS 60
#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 420
#define APPLICATION_TITLE "Voxel Lab"

#include "../include/app.h"
#include "../include/mouse.h"
#include "../include/ray.h"
#include "../include/color.h"
#include "../include/sparce_voxel_octree.h"
#include "../include/voxel_octree.h"
#include "../include/graphical_debugger.h"
#include "../include/stack.h"


enum tool{TOOL_PAINT, TOOL_ERASE, TOOL_SELECT};
enum object_type{VOXEL_OBJECT};

float gScrollY;
GLFWscrollfun scroll_callback(GLFWwindow* window, double scrollX, double scrollY){
    gScrollY = scrollY;
}

int main(int argc, char const *argv[]){
    struct nk_glfw glfw = {0};
    GLFWwindow* window;    
    struct nk_context *ctx;
    App *app;    
    Mouse *mouse = (Mouse*) malloc(sizeof(Mouse));
    GLuint rayHitVertexBuffer, rayHitElementBuffer;
    app = (App*) malloc(sizeof(App));
    World *world = &app->world;
    ColorRGBAF *bgColor = &world->bgColor;
    FrameBufferObject *appFrame = &app->appViewFrame; 
    float time, deltaTime;
    int selectedIndex, selectedObjectType;
    SparceVoxelOctreeEditor *svoEditor;
    VoxelOctreeEditor *voEditor;
    GraphicalDebugger *graphicalDebugger;
    

    //Init GLFW
    if (!glfwInit()){
        return -1;
    }

    window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, APPLICATION_TITLE, NULL, NULL);
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

    //Init Voxel Editor
    svoEditor = (SparceVoxelOctreeEditor*) malloc(sizeof(SparceVoxelOctreeEditor));
    init_colorRgba(&svoEditor->brushColor ,0, 255, 0, 255);
    voEditor = (VoxelOctreeEditor*) malloc(sizeof(VoxelOctreeEditor));

    //Init Graphical Debugger
    graphicalDebugger = (GraphicalDebugger*) malloc(sizeof(GraphicalDebugger));
    graphicalDebugger->debugItems = NULL;
    glGenBuffers(1,&graphicalDebugger->vertexBuffer);
    glGenBuffers(1,&graphicalDebugger->elementBuffer);


    //Init nuklear
    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);    
    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);
        nk_glfw3_font_stash_end(&glfw);
    }    
    glfwSetScrollCallback(window, scroll_callback);

    //Init Shaders
    app->flatShader = shader_generate("shaders/flat.vert", "shaders/flat.frag");
    app->phongShader = shader_generate("shaders/phong.vert", "shaders/phong.frag");

    
    //Init FrameBuffer
    glGenFramebuffers(1, &appFrame->buffer);
    glGenTextures(1, &appFrame->colorTexture);
    glGenTextures(1, &appFrame->depthStencilTexture);  
    frame_buffer_object_update(appFrame, 640, 480);

    
    //Init Mouse
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        mouse->prevX = x;
        mouse->prevY = y;
        mouse->x = x;
        mouse->y = y;
        mouse->dx = 0;
        mouse->dy = 0;
        mouse->primaryButton.state = BUTTON_STATE_UP;
        mouse->secondaryButton.state = BUTTON_STATE_UP;   
    }

    //Init Camera
    {
        Camera *camera = &world->viewCamera;
        camera->fov = M_PI_2;
        camera->near = 1.0f;
        camera->far = 100.0f;
        camera->pitch = 0;
        camera->yaw = 0;
        camera->offset[0] = 0;
        camera->offset[1] = 0;
        camera->offset[2] = 0;
        camera->distance = 10; 
    }

    //Init Floor Grid
    {
        FloorGrid *floorGrid = &world->floorGrid;
        vec3 min = {0,0,0};
        vec3 vecH = {1,0,0};
        vec3 vecV = {0,0,1};
        int v = 16;
        int h = 16;  
        grid_init(&floorGrid->grid, min, vecH, vecV, v, h);
        int vertexCount =  floor_grid_vertex_count(*floorGrid);
        int floatCount = 7 * vertexCount;
        int gridSize = floatCount * sizeof(float);
        floorGrid->vertexArray = (float*) malloc(gridSize);

        //Initialize Floor Grid Model
        floor_grid_generate_vertex_array(floorGrid, floorGrid->vertexArray);

        glGenBuffers(1, &floorGrid->vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, floorGrid->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, gridSize, floorGrid->vertexArray,
            GL_DYNAMIC_DRAW
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        

    }
    
    //Init ray hit model
    glGenBuffers(1, &rayHitVertexBuffer);
    glGenBuffers(1, &rayHitElementBuffer);

    
    //Init World
    //Set Background Color
    color_f_init(bgColor, 0.5, 0.5, 0.5, 1.0);

    // Add Initial Voxel Object
    world->voxelObjectCount = 0;
    world->voxelObject = NULL;
    world_add_voxel_object(world);

    //Init some app properties
    app->toolSelected = TOOL_PAINT;

    time = glfwGetTime();
    deltaTime = 1 / FPS;
    while (!glfwWindowShouldClose(window)){
        int width, height;

        glfwGetWindowSize(window, &width, &height);        
        glViewport(0,0, width, height);
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        mouse->primaryButton.isDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
        mouse->secondaryButton.isDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);
        
        //Update Mouse Button States
        mouse->primaryButton.state = button_get_next_state(
            mouse->primaryButton.state,
            mouse->primaryButton.isDown
        );

        mouse->secondaryButton.state = button_get_next_state(
            mouse->secondaryButton.state,
            mouse->secondaryButton.isDown
        );
    
        //Update The Mouse Position
        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            mouse->dx = x - mouse->x;
            mouse->dy = y - mouse->y;
            mouse->prevX = mouse->x;
            mouse->prevY = mouse->y;
            mouse->x = x;
            mouse->y = y;            
        }
        
        glfwPollEvents();
        
        nk_glfw3_new_frame(&glfw);
       

        if(nk_begin(ctx, "Gui Window", nk_rect(0,0, width, height),
            NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND | NK_WINDOW_BORDER
        )){
            nk_menubar_begin(ctx);
            nk_layout_row_template_begin(ctx, 32);
            nk_layout_row_template_push_static(ctx, strlen("File") * 8);
            nk_layout_row_template_push_static(ctx, strlen("About") * 8);
            nk_layout_row_template_end(ctx);

            struct nk_vec2 v = {64, 32 * 8};
            if(nk_menu_begin_label(ctx, "File", NK_TEXT_ALIGN_CENTERED, v)){
                nk_layout_row_dynamic(ctx, 25,1);
                nk_menu_item_label(ctx, "New", NK_TEXT_LEFT);
                nk_menu_item_label(ctx, "Save As", NK_TEXT_LEFT);
                nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT);
                nk_menu_item_label(ctx, "Quit", NK_TEXT_LEFT);
                nk_menu_end(ctx);
            }

            if(nk_menu_begin_label(ctx, "About", NK_TEXT_ALIGN_CENTERED, v)){
                nk_layout_row_dynamic(ctx, 25,1);
                nk_menu_item_label(ctx, "Git Repo", NK_TEXT_LEFT);
                nk_menu_end(ctx);
            }
            
            nk_menubar_end(ctx);

            nk_layout_row_template_begin(ctx, height - 32);
            nk_layout_row_template_push_static(ctx, 64);
            nk_layout_row_template_push_static(ctx, 128);
            nk_layout_row_template_push_static(ctx, width - 64 - 128);
            nk_layout_row_template_end(ctx);
            
            if(nk_group_begin(ctx, "Panel 0", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){ 
                nk_layout_row_static(ctx, 24, 48, 1);
                if(nk_select_label(ctx, "Tool", NK_TEXT_ALIGN_CENTERED, app->propertySelected == PROPERTY_TOOL)){
                    app->propertySelected = PROPERTY_TOOL;
                }

                if(nk_select_label(ctx, "Cam", NK_TEXT_ALIGN_CENTERED, app->propertySelected == PROPERTY_CAMERA)){
                    app->propertySelected= PROPERTY_CAMERA;
                }

                if(nk_select_label(ctx, "Sce", NK_TEXT_ALIGN_CENTERED, app->propertySelected == PROPERTY_SCENE)){
                    app->propertySelected = PROPERTY_SCENE;
                }

                nk_group_end(ctx);
            }
            
            float panel1Height = nk_widget_height(ctx);
            if(nk_group_begin(ctx, "Panel 1", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
                Camera *camera = &world->viewCamera;
                //nk_layout_row_dynamic(ctx, panel1Height / 1.9,1);
                if(nk_tree_push(ctx, NK_TREE_NODE, "Tools", NK_MINIMIZED)){
                    nk_layout_row_dynamic(ctx, 32,3);
                    if(nk_select_label(ctx, "paint", NK_TEXT_ALIGN_CENTERED, app->toolSelected == TOOL_PAINT)){
                        app->toolSelected = TOOL_PAINT;
                    }
                    if(nk_select_label(ctx, "erase", NK_TEXT_ALIGN_CENTERED, app->toolSelected == TOOL_ERASE)){
                        app->toolSelected = TOOL_ERASE;
                    }
                    if(nk_select_label(ctx, "select", NK_TEXT_ALIGN_CENTERED, app->toolSelected == TOOL_SELECT)){
                        app->toolSelected = TOOL_SELECT;
                    }
                    if(nk_tree_push(ctx, NK_TREE_NODE, "Properties", NK_MINIMIZED)){
                        nk_layout_row_dynamic(ctx, 32,2);
                        switch (app->toolSelected)
                        {
                        case TOOL_PAINT:
                            break;
                        case TOOL_ERASE:
                            break;
                        case TOOL_SELECT:
                            break;
                        }

                        nk_tree_pop(ctx);
                    }

                    nk_tree_pop(ctx);
                }
                if(nk_tree_push(ctx, NK_TREE_NODE, "Explorer", NK_MINIMIZED)){
                    if(nk_tree_push(ctx, NK_TREE_NODE, "World", NK_MINIMIZED)){
                        nk_layout_row_dynamic(ctx, 32,1);
                        if(nk_button_label(ctx, "add voxel object")){
                            world_add_voxel_object(world);
                        }
                        for(int i = 0; i < world->voxelObjectCount; i++){
                            if(nk_select_label(ctx, "Voxel Object", NK_TEXT_ALIGN_CENTERED, 
                                (selectedIndex == i) && (selectedObjectType == VOXEL_OBJECT))){
                                selectedIndex = i;
                                selectedObjectType = VOXEL_OBJECT;
                            }
                        }
                        nk_tree_pop(ctx);

                    }

                    nk_tree_pop(ctx);
                }
                if(nk_tree_push(ctx, NK_TREE_NODE, "Properties", NK_MINIMIZED)){
                    nk_layout_row_dynamic(ctx, 32, 1);
                    nk_tree_pop(ctx);
                }

                nk_layout_row_static(ctx, 128, 128, 1);
                //Update the brush color
                {
                    //TO nuklear color;
                    ColorRGBAF c = colorRgba_to_colorRgbaf(svoEditor->brushColor);
                    struct nk_colorf pickColor = {c.r, c.g, c.b, c.a}; 
                    nk_color_pick(ctx, &pickColor, NK_FORMAT_RGBA32);
                    ColorRGBA color = colorRgbaf_to_colorRgba(*((ColorRGBAF*)&pickColor));
                    colorRgba_copy(&svoEditor->brushColor, color);
                }
                nk_group_end(ctx);
                
            }
            
         
            struct nk_rect appRect = nk_widget_bounds(ctx);
            if(nk_group_begin(ctx, "App View", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
                Camera *camera = &world->viewCamera;
                mat4x4 projMat, viewMat, inverseMat;
                Ray worldRay;
                int worldHit, worldHitSide;
                float worldHitDistance = 1000;
                vec3 worldHitPoint;
                frame_buffer_object_update(appFrame, (int)appRect.w, (int)appRect.h);

                glBindFramebuffer(GL_FRAMEBUFFER, appFrame->buffer);

                glViewport(0,0, (int)appRect.w, (int)appRect.h);
                glClearColor(bgColor->r, bgColor->g, bgColor->b, bgColor->a);
                glEnable(GL_DEPTH_TEST);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                //Update The Camera Rotation;
                if(mouse->secondaryButton.isDown){
                    camera->yaw += deltaTime * mouse->dx;
                    camera->pitch += deltaTime * mouse->dy;
                }            

                camera->distance += gScrollY;
                
                //Update projection matrix from camera
                {
                    float aspectRatio = appRect.w / appRect.h;
                    mat4x4_perspective(projMat, camera->fov, aspectRatio,
                        camera->near, camera->far
                    );
                }

                //Update view matrix from camera
                camera_get_view_matrix(*camera, viewMat);
                
                /*Raycast From Camera*/
                //Set the ray to start at Camera Position;
                camera_get_position(*camera, worldRay.origin);

                /*Calculate the ray directions*/
                //Get Ray Direction
                {
                    mat4x4 projView;
                    mat4x4_mul(projView, projMat, viewMat);
                    mat4x4_invert(inverseMat, projView);

                    float ux = 2 * (mouse->x - appRect.x) /
                        appRect.w - 1;

                    float uy = -2 * (mouse->y - appRect.y) /
                        appRect.h + 1;

                    vec4 clipV = {ux, -uy, -1.0, 1.0};
                    vec4 targetV;
                    mat4x4_mul_vec4(targetV, inverseMat, clipV);
                    vec3 dirV;
                    vec3_sub(dirV, targetV, worldRay.origin);
                    vec3_norm(worldRay.direction, dirV);
                }

                //Raycast
                worldHit = 0;
                worldHit = ray_vs_y0(worldRay, &worldHitDistance, &worldHitSide);
                
                for(int i = 0; i < world->voxelObjectCount; i++){

                    VoxelOctreeTree *collider = &world->voxelObject[i].collider;
                    worldHit = ray_vs_voxel_octree(worldRay, collider, &worldHitDistance, &worldHitSide) || worldHit;
                }

                if(worldHit){
                    vec3_scale(worldHitPoint, worldRay.direction, worldHitDistance);
                    vec3_add(worldHitPoint, worldHitPoint, worldRay.origin);
                }

                //World Edit
                int appRectTest = rect_contains_point(
                    rect_init(appRect.x, appRect.y, appRect.w, appRect.h),
                    mouse->x, mouse->y
                );

                if(mouse->primaryButton.state == BUTTON_STATE_PRESSED && appRectTest){
                    switch (selectedObjectType)
                    {
                    case VOXEL_OBJECT:
                        {
                            int objectExists = (selectedIndex >= 0) && (selectedIndex < world->voxelObjectCount);
                            if(objectExists){
                                svoEditor->action = (app->toolSelected == TOOL_ERASE) * VOXEL_DELETE;
                                svoEditor->action += (app->toolSelected == TOOL_PAINT) * VOXEL_ADD;
                                svoEditor->shapeType = SHAPE_AABB;

                                voEditor->action = (app->toolSelected == TOOL_ERASE) * VOXEL_DELETE;
                                voEditor->action += (app->toolSelected == TOOL_PAINT) * VOXEL_ADD;
                                voEditor->shapeType = SHAPE_AABB;
                                {
                                    float cellX, cellY, cellZ;
                                    switch (worldHitSide)
                                    {
                                    case RAY_HIT_SIDE_LEFT:
                                        cellX = roundf(worldHitPoint[0]) -1;
                                        cellY = floorf(worldHitPoint[1]);
                                        cellZ = floorf(worldHitPoint[2]);
                                        break;
                                    case RAY_HIT_SIDE_RIGHT:
                                        cellX = roundf(worldHitPoint[0]);
                                        cellY = floorf(worldHitPoint[1]);
                                        cellZ = floorf(worldHitPoint[2]);
                                        break;
                                    case RAY_HIT_SIDE_BOTTOM:
                                        cellX = floorf(worldHitPoint[0]);
                                        cellY = roundf(worldHitPoint[1]) -1;
                                        cellZ = floorf(worldHitPoint[2]);
                                        break;
                                    case RAY_HIT_SIDE_TOP:
                                        cellX = floorf(worldHitPoint[0]);
                                        cellY = roundf(worldHitPoint[1]);
                                        cellZ = floorf(worldHitPoint[2]);
                                        break;
                                    case RAY_HIT_SIDE_FRONT:
                                        cellX = floorf(worldHitPoint[0]);
                                        cellY = floorf(worldHitPoint[1]);
                                        cellZ = roundf(worldHitPoint[2]) -1;
                                        break;
                                    case RAY_HIT_SIDE_BACK:
                                        cellX = floorf(worldHitPoint[0]);
                                        cellY = floorf(worldHitPoint[1]);
                                        cellZ = roundf(worldHitPoint[2]);
                                        break;
                                    }
                                    float aabb[6] = {cellX, cellY, cellZ, 1, 1, 1};
                                    svoEditor->shapeData = &aabb;
                                    voEditor->ShapeData = &aabb;
                                    VoxelObject *voxelObject = &world->voxelObject[selectedIndex];
                                    sparce_voxel_octree_edit(&voxelObject->graphic, svoEditor);
                                    voxel_octree_edit(&voxelObject->collider, voEditor);
                                }
                                
                            }
                        }
                        break;
                    
                    }
                }

                glUseProgram(app->flatShader);
                glUniformMatrix4fv(
                    glGetUniformLocation(app->flatShader,"uProjMat"),
                    1, GL_FALSE, (float*)projMat
                );
                glUniformMatrix4fv(
                    glGetUniformLocation(app->flatShader,"uViewMat"),
                    1, GL_FALSE, (float*)viewMat
                );
       
                {
                    mat4x4 idMat;
                    mat4x4_identity(idMat);
                    glUniformMatrix4fv(
                        glGetUniformLocation(app->flatShader,"uModelMat"),
                        1, GL_FALSE, (float*)idMat
                    );
                }
           
                //Draw Ray Hit
                if(worldHit){
                    //glBindBuffer(GL_ARRAY_BUFFER, rayHitVertexBuffer);
                    {
                        Aabb aabb = {
                            worldHitPoint[0] - 0.25, 
                            worldHitPoint[1] - 0.25,
                            worldHitPoint[2] - 0.5,0.5,0.5,0.5
                        };
                        ColorRGBAF yellow = {1.0,1.0,0.0,1.0};
                        graphical_debugger_push_aabb(graphicalDebugger, aabb, yellow);
                    }
                }
                
                //Debug Voxels
                #define DEBUG_VOXEL
                #define DEBUG_OCTREE
                #define DEBUG_SPARCE_OCTREE
                #ifdef DEBUG_VOXEL
                for(int i = 0; i < world->voxelObjectCount; i++){
                    
                    if(world->voxelObject[i].collider.head){
                        ColorRGBAF color = {1,0,1,1};
                        #ifdef DEBUG_OCTREE
                        graphic_debugger_push_voxel(
                            graphicalDebugger,
                            world->voxelObject[i].collider.head,
                            color
                        );
                        #endif
                    }
                }
                #endif
                
                //Draw GraphicalDebugger
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                graphical_debugger_draw(graphicalDebugger);
                graphical_debugger_clear(graphicalDebugger);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                //Debug Voxels TYpe 2
                for(int i = 0; i < world->voxelObjectCount; i++){
                    if(world->voxelObject[i].graphic.head){
                        ColorRGBAF color = {0,0,1,1};
                        #ifdef DEBUG_SPARCE_OCTREE
                        graphical_debugger_push_sparce_voxel(
                            graphicalDebugger,
                            world->voxelObject[i].graphic.head,
                            color 
                        );
                        #endif
                    }
                }
                graphical_debugger_draw(graphicalDebugger);
                graphical_debugger_clear(graphicalDebugger);
                

                //Draw floorGrid
                {
                    FloorGrid *floorGrid = &world->floorGrid;
                    glBindBuffer(GL_ARRAY_BUFFER, floorGrid->vertexBuffer);
                    glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(sizeof(float) * 3));
                    glEnableVertexAttribArray(1);
                    glDrawArrays(GL_LINES, 0, floor_grid_vertex_count(*floorGrid));
                }

                glUseProgram(app->phongShader);
                glUniformMatrix4fv(
                    glGetUniformLocation(app->phongShader,"uProjMat"),
                    1, GL_FALSE, (float*)projMat
                );
                glUniformMatrix4fv(
                    glGetUniformLocation(app->phongShader,"uViewMat"),
                    1, GL_FALSE, (float*)viewMat
                );
       
                {
                    mat4x4 idMat;
                    mat4x4_identity(idMat);
                    glUniformMatrix4fv(
                        glGetUniformLocation(app->phongShader,"uModelMat"),
                        1, GL_FALSE, (float*)idMat
                    );
                }
                
                //All Visible VoxelModels
                for(int i = 0; i < world->voxelObjectCount; i++){
                    VoxelObject *voxelObject = &world->voxelObject[i];
                    if(voxelObject->graphic.head != NULL){
                        //Draw The Voxel Chunks
                    }
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                //Render The App to a Nuklear Texture
                {
                    struct nk_image image = nk_image_id(appFrame->colorTexture);
                    nk_layout_row_dynamic(ctx, appRect.h, 1);
                    nk_image_color(ctx, image, nk_white);
                }
                
                nk_group_end(ctx);
            }
        }
        nk_end(ctx);
    
        gScrollY = 0;
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(window);
        {
            float newTime = glfwGetTime();
            deltaTime = newTime- time;
            time = newTime;
        }

    } 
    nk_glfw3_shutdown(&glfw);
    glfwTerminate();
    
    return 0;
}
