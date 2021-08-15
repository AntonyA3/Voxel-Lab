
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
#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 420
#define DEFAULT_PROP_TAB_ITEM_COUNT 3
#define DEFAULT_MENU_TAB_ITEM_COUNT 2
#define DEFAULT_MENU_BAR_HEIGHT 32
#define DEFAULT_PROPERTY_WIDTH 128
#define DEFAULT_PROPERTY_SELECTOR_WIDTH 48
#define DEFAULT_HIT_DISTANCE 1000
#define APPLICATION_TITLE "Voxel Lab"
#define TEXTURE_Y_FLIPPED

#include "../include/app.h"
#include "../include/menu_bar_item.h"
#include "../include/camera.h"
#include "../include/voxel_tree.h"

static vec3 VEC3_UP = {0, 1, 0 };
static vec3 VEC3_RIGHT = {1, 0, 0};
static vec3 VEC3_FORWARD = {0, 0, 1};

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
GLuint shader_generate(const char *vertexShaderPath, const char *fragmentShaderPath){
    const char *vertexShaderText;
    const char *fragmentShaderText;
    int success;
    char infoLog[1000];
    file_get_text(vertexShaderPath, &vertexShaderText);
    file_get_text(fragmentShaderPath, &fragmentShaderText);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1, (const char**)&vertexShaderText,NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(vertexShader, 1000, NULL, infoLog);
    if(!success){
        printf("vertex shader failed to load \n");
        printf("message: %s\n", infoLog);
    }
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char**)&fragmentShaderText, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(vertexShader, 1000, NULL,infoLog);
    if(!success){
        printf("fragment shader failed to load \n");
        printf("message: %s\n", infoLog);
    }
  
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    glGetProgramInfoLog(program, 1000, NULL, infoLog);
    if(!success){
        printf("program failed to load \n");
        printf("message: %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;    
}

int main(int argc, char const *argv[])
{
    struct nk_glfw glfw = {0};
    struct nk_context *ctx;
    float deltaTime = 1 / FPS;
    float time = 0;
    GLFWwindow* window;
    App app;


    if (!glfwInit()){
        return -1;
    }

    window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, APPLICATION_TITLE, NULL, NULL);
    app.guiWindow.width = DEFAULT_WINDOW_WIDTH;
    app.guiWindow.height = DEFAULT_WINDOW_HEIGHT;
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
    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);    
    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);
        nk_glfw3_font_stash_end(&glfw);
    }      
    glfwSetScrollCallback(window, scroll_callback);

    app.flatShader = shader_generate("shaders/flat.vert", "shaders/flat.frag");
    app.phongShader = shader_generate("shaders/phong.vert", "shaders/phong.frag");
    app.mainVoxels.head = NULL;

    glGenFramebuffers(1, &app.frame.buffer);
    glGenTextures(1, &app.frame.colorTexture);
    glGenTextures(1, &app.frame.depthStencilTexture);
    
    app.propertySelected = PROPERTY_CAMERA;
    
    app.camera.yaw = 0;
    app.camera.pitch = 0;
    app.camera.distance = 10;
    app.camera.near = 1.0;
    app.camera.far = 100.0;
    app.camera.aspectRatio = 1.0;
    app.camera.fov = M_PI_2;
    
    frame_update(&app.frame, 
        app.guiWindow.windowContent.appView.width,
        app.guiWindow.windowContent.appView.height
    );


    app.toolSelected = TOOL_PAINT;

    app.scene.lightYaw = 0;
    app.scene.lightPitch = 0;
        
    app.floorGrid.min[0] = 0;
    app.floorGrid.min[1] = 0;
    app.floorGrid.min[2] = 0;
    app.floorGrid.vectorH[0] = 1;
    app.floorGrid.vectorH[1] = 0;
    app.floorGrid.vectorH[2] = 0;

    app.floorGrid.vectorV[0] = 0;
    app.floorGrid.vectorV[1] = 0;
    app.floorGrid.vectorV[2] = 1;

    app.floorGrid.vCount = 32;
    app.floorGrid.hCount = 32;     
    for(int i = 0; i < MODEL_COUNT; i++){
        app.models[i].vertexArray = (float*) malloc(sizeof(float));
        app.models[i].elementArray = (unsigned int*) malloc(sizeof(unsigned int)); 
        glGenBuffers(1, &app.models[i].vertexBuffer);
        glGenBuffers(1, &app.models[i].elementBuffer);
    }
    
    {
        vec4 color = {1.0,1.0,1.0,1.0};
        model_generate(MODEL_TYPE_GRID, &app.floorGridModel, &app.floorGrid,color, 
            VERTEX_POSITION + VERTEX_RGBA, GL_DYNAMIC_DRAW
        );

        model_generate(MODEL_TYPE_TEST_TRI, &app.testTriangle, NULL, color, VERTEX_POSITION + VERTEX_RGBA,
        GL_DYNAMIC_DRAW);
    }
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        app.cursor.prevX = x; 
        app.cursor.prevY = y;
        app.cursor.x = x;
        app.cursor.y = y;
    }
    app.cursor.dx = 0;
    app.cursor.dy = 0;
    
    app.scene.lightDirection[0] = 1;
    app.scene.lightDirection[1] = 0;
    app.scene.lightDirection[2] = 0;
    for(int i = 0; i < 2; i++){
        app.cursor.mouseButtons[i].state = BUTTON_STATE_UP;
    }
    
    time = glfwGetTime();
    
    while (!glfwWindowShouldClose(window)){
        {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            gui_window_update(&app.guiWindow, (float)width, (float)height);
        }   
        app.guiWindow.menuBar.height = DEFAULT_MENU_BAR_HEIGHT;
        app.guiWindow.windowContent.propTab.width = DEFAULT_PROPERTY_WIDTH;
        app.guiWindow.windowContent.selectorTab.width = DEFAULT_PROPERTY_SELECTOR_WIDTH ;
        
        app.guiWindow.windowContent.appView.x = 
            app.guiWindow.windowContent.propTab.width + 
            app.guiWindow.windowContent.selectorTab.width;
        app.guiWindow.windowContent.appView.y = app.guiWindow.menuBar.height;
        app.guiWindow.windowContent.appView.width = app.guiWindow.width - 
            app.guiWindow.windowContent.propTab.width -
            app.guiWindow.windowContent.selectorTab.width;
        app.guiWindow.windowContent.appView.height = 
            app.guiWindow.height - 
            app.guiWindow.menuBar.height;            

    
        frame_update(&app.frame, 
            app.guiWindow.windowContent.appView.width,
            app.guiWindow.windowContent.appView.height
        );
    

        app.cursor.primaryButton.isDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
        app.cursor.secondaryButton.isDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);

        {
            quat rotQuat, yawQuat, pitchQuat;
            vec4 dir1 = {0,0,1};
            quat_rotate(yawQuat, app.scene.lightYaw, VEC3_UP);
            quat_rotate(pitchQuat, app.scene.lightPitch, VEC3_RIGHT);
            quat_mul(rotQuat, yawQuat, pitchQuat);
            quat_mul_vec3(app.scene.lightDirection, rotQuat, dir1);
        }

        for(int i = 0; i < 2; i++){
            int isDown = app.cursor.mouseButtons[i].isDown;
            switch (app.cursor.mouseButtons[i].state)
            {
            case BUTTON_STATE_DOWN:
                if(!isDown){
                    app.cursor.mouseButtons[i].state = BUTTON_STATE_RELEASED;
                }           
                break;
            case BUTTON_STATE_PRESSED:
                if(isDown){
                    app.cursor.mouseButtons[i].state = BUTTON_STATE_DOWN;
                }else{
                    app.cursor.mouseButtons[i].state = BUTTON_STATE_RELEASED;
                }
                break;
            case BUTTON_STATE_RELEASED:
                if(isDown){
                    app.cursor.mouseButtons[i].state = BUTTON_STATE_PRESSED;
                }else{
                    app.cursor.mouseButtons[i].state = BUTTON_STATE_UP;
                }
                break;
            case BUTTON_STATE_UP:
                if(isDown){
                    app.cursor.mouseButtons[i].state = BUTTON_STATE_PRESSED;
                }
                break;
            }
        }
        

        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            app.cursor.dx = x - app.cursor.prevX;
            app.cursor.dy = y - app.cursor.prevY;
            app.cursor.prevX = app.cursor.x;
            app.cursor.prevY = app.cursor.y;
            app.cursor.x = x;
            app.cursor.y = y;
        }
        app.cursor.clipX = 2 * (app.cursor.x - app.guiWindow.windowContent.appView.x) /
            app.guiWindow.windowContent.appView.width - 1;
        app.cursor.clipY = -2 * (app.cursor.y - app.guiWindow.windowContent.appView.y) /
            app.guiWindow.windowContent.appView.height + 1;
        memcpy(app.cursor.ray.origin, app.camera.position, sizeof(vec3));        
    
        
        app.camera.aspectRatio = 
            app.guiWindow.windowContent.appView.width /
            app.guiWindow.windowContent.appView.height;
        app.isCameraRotate = app.cursor.secondaryButton.isDown;
        
        if(app.isCameraRotate){
            app.camera.yaw += deltaTime * app.cursor.dx;
            app.camera.pitch += deltaTime * app.cursor.dy;
        }            
        app.camera.distance += globalVars.scrollY;
        globalVars.scrollY = 0;

        mat4x4_perspective(app.camera.projMat, 
            app.camera.fov,
            app.camera.aspectRatio, 
            app.camera.near, app.camera.far
        );
        
        quat_rotate(app.camera.yawQuat, app.camera.yaw, VEC3_UP);
        quat_rotate(app.camera.pitchQuat, app.camera.pitch, VEC3_RIGHT);
        quat_rotate(app.camera.rollQuat, M_PI, VEC3_FORWARD);

        
        {
            quat yawQuat, pitchQuat, rollQuat, rotationQuat, rotationQuat2;
            mat4x4 rotationMatrix, distanceMatrix, offsetMatrix;
            
            mat4x4_translate(offsetMatrix, 
                -app.camera.offset[0],
                -app.camera.offset[1], 
                -app.camera.offset[2]
            );
            
            quat_rotate(yawQuat, -app.camera.yaw, VEC3_UP);
            quat_rotate(pitchQuat, -app.camera.pitch, VEC3_RIGHT);
            quat_rotate(rollQuat, M_PI, VEC3_FORWARD);
            quat_mul(rotationQuat, yawQuat, rollQuat);
            quat_mul(rotationQuat2,pitchQuat, rotationQuat);
            mat4x4_from_quat(rotationMatrix, rotationQuat2);
            
            mat4x4_translate(distanceMatrix, 0, 0, -app.camera.distance);

            mat4x4_mul(app.camera.viewMat, rotationMatrix,offsetMatrix);
            mat4x4_mul(app.camera.viewMat, distanceMatrix, app.camera.viewMat);
        }
        {
            vec4 identityVector = {0, 0, 0, 1}, distanceVector, rotatedVector, offsetVector;
            quat rotationQuat, rotationQuat2;
            mat4x4 distanceMatrix, rotationMatrix, offsetMatrix;

            quat_mul(rotationQuat, app.camera.yawQuat, app.camera.pitchQuat );
            quat_mul(rotationQuat2, app.camera.rollQuat, rotationQuat);
            mat4x4_from_quat(rotationMatrix, rotationQuat2);
            mat4x4_translate(distanceMatrix, 0, 0, app.camera.distance);
            mat4x4_translate(offsetMatrix, app.camera.offset[0], app.camera.offset[1], app.camera.offset[2]);

            mat4x4_mul_vec4(distanceVector, distanceMatrix, identityVector);
            mat4x4_mul_vec4(rotatedVector, rotationMatrix, distanceVector);
            mat4x4_mul_vec4(offsetVector, offsetMatrix, rotatedVector);
            memcpy(app.camera.position, offsetVector, sizeof(vec3));
            memcpy(app.cursor.ray.origin, app.camera.position, sizeof(vec3));
        }
        
        {
            mat4x4 projView;
            mat4x4_mul(projView, app.camera.projMat, app.camera.viewMat);
            mat4x4_invert(app.camera.inverseMat, projView);
            
            vec4 clipV = {app.cursor.clipX, -app.cursor.clipY, -1.0, 1.0};
            vec4 targetV;
            mat4x4_mul_vec4(targetV, app.camera.inverseMat, clipV);
            vec3 dirV;
            vec3_sub(dirV, targetV, app.cursor.ray.origin);
            vec3_norm(app.cursor.ray.direction, dirV);
            
        }
                
        app.cursor.isHit = 0;
        app.cursor.hitDistance = DEFAULT_HIT_DISTANCE;
        app.cursor.hitSide = 0;

        app.cursor.isHit = ray_vs_y0(app.cursor.ray, &app.cursor.hitDistance, &app.cursor.hitSide);
        if(app.cursor.isHit){
            app.cursor.hitEntity = RAY_HIT_Y0;
        }
        
        {
            int voxelModelHit = ray_vs_voxel_tree(app.cursor.ray, &app.mainVoxels, &app.cursor.hitDistance, &app.cursor.hitSide);
            if(voxelModelHit){
                app.cursor.hitEntity = RAY_HIT_VOXEL_TREE;
            }
            app.cursor.isHit = app.cursor.isHit || voxelModelHit;
        }
        
        if(app.cursor.isHit){
            vec3_scale(app.cursor.hit, app.cursor.ray.direction, app.cursor.hitDistance);
            vec3_add(app.cursor.hit, app.cursor.hit, app.cursor.ray.origin); 
        }
            
        if(app.cursor.isHit){
            switch (app.cursor.hitEntity)
            {
            case RAY_HIT_Y0:
                switch (app.toolSelected)
                {
                case TOOL_PAINT:
                    app.voxelEditor.cell[0] = floorf(app.cursor.hit[0]);
                    app.voxelEditor.cell[1] = 0 - 1 *(app.cursor.hitSide == RAY_HIT_SIDE_BOTTOM);
                    app.voxelEditor.cell[2] = floorf(app.cursor.hit[2]);
                    break;
                case TOOL_ERASE:
                    app.voxelEditor.cell[0] = floorf(app.cursor.hit[0]);
                    app.voxelEditor.cell[1] = 0 - 1 *(app.cursor.hitSide == RAY_HIT_SIDE_TOP);
                    app.voxelEditor.cell[2] = floorf(app.cursor.hit[2]);
                    break;
                }
                
                break;
            case RAY_HIT_VOXEL_TREE:
                switch (app.cursor.hitSide)
                {
                case RAY_HIT_SIDE_LEFT:
                    app.voxelEditor.cell[0] = roundf(app.cursor.hit[0]) - 1 * (app.toolSelected == TOOL_PAINT);
                    app.voxelEditor.cell[1] = floorf(app.cursor.hit[1]);
                    app.voxelEditor.cell[2] = floorf(app.cursor.hit[2]); 
                    break;
                case RAY_HIT_SIDE_RIGHT:
                    app.voxelEditor.cell[0] = roundf(app.cursor.hit[0]) -1 * (app.toolSelected == TOOL_ERASE);
                    app.voxelEditor.cell[1] = floorf(app.cursor.hit[1]);
                    app.voxelEditor.cell[2] = floorf(app.cursor.hit[2]); 
                    break;
                case RAY_HIT_SIDE_BOTTOM:
                    app.voxelEditor.cell[0] = floorf(app.cursor.hit[0]);
                    app.voxelEditor.cell[1] = roundf(app.cursor.hit[1]) - 1 * (app.toolSelected == TOOL_PAINT);
                    app.voxelEditor.cell[2] = floorf(app.cursor.hit[2]); 
                    break;
                case RAY_HIT_SIDE_TOP:
                    app.voxelEditor.cell[0] = floorf(app.cursor.hit[0]);
                    app.voxelEditor.cell[1] = roundf(app.cursor.hit[1]) -1 * (app.toolSelected == TOOL_ERASE);
                    app.voxelEditor.cell[2] = floorf(app.cursor.hit[2]); 
                    break;
                case RAY_HIT_SIDE_FRONT:
                    app.voxelEditor.cell[0] = floorf(app.cursor.hit[0]);
                    app.voxelEditor.cell[1] = floorf(app.cursor.hit[1]);
                    app.voxelEditor.cell[2] = roundf(app.cursor.hit[2]) - 1 * (app.toolSelected == TOOL_PAINT); 
                    break;
                case RAY_HIT_SIDE_BACK:
                    app.voxelEditor.cell[0] = floorf(app.cursor.hit[0]);
                    app.voxelEditor.cell[1] = floorf(app.cursor.hit[1]);
                    app.voxelEditor.cell[2] = roundf(app.cursor.hit[2]) -1 * (app.toolSelected == TOOL_ERASE); 
                    break;
                }
                
                break;
            }
        }
        if(app.cursor.primaryButton.state == BUTTON_STATE_PRESSED){
            float aabb[6] = {app.voxelEditor.cell[0], app.voxelEditor.cell[1],
                    app.voxelEditor.cell[2], 1.0, 1.0, 1.0
                };
            switch (app.toolSelected)
            {
            case TOOL_PAINT:
                voxel_tree_editor(&app.mainVoxels, SHAPE_AABB, aabb, VOXEL_ACTION_ADD);
                break;
            case TOOL_ERASE:
                voxel_tree_editor(&app.mainVoxels, SHAPE_AABB, aabb, VOXEL_ACTION_DELETE);
                break;
            }
            app.mainVoxels.voxelCount = voxel_count(app.mainVoxels.head);

            vec4 color = {0.0,1.0,0.0,1.0};

            model_generate(MODEL_TYPE_VOXEL_MODEL, &app.voxelModel, &app.mainVoxels, color,
                VERTEX_POSITION + VERTEX_RGBA + VERTEX_NORMAL,
                GL_DYNAMIC_DRAW
            );
        }

        if(app.cursor.isHit){
            Aabb aabb = {
                app.cursor.hit[0] - 0.5, 
                app.cursor.hit[1] - 0.5, 
                app.cursor.hit[2] - 0.5
                ,0.5, 0.5, 0.5
            };
            vec4 color = {1.0,1.0,0.0,1.0};
            
            model_generate(MODEL_TYPE_AABB, &app.hitModel, 
                &aabb, 
                &color,
                VERTEX_POSITION + VERTEX_RGBA,
                GL_DYNAMIC_DRAW
            );

            
        }
        if(app.mainVoxels.head != NULL){
            vec4 color = {0.0,1.0,0.0,1.0};
            Aabb aabb = voxel_to_aabb(*app.mainVoxels.head);
            model_generate(MODEL_TYPE_AABB, &app.voxelBox, 
                &aabb, 
                &color,
                VERTEX_POSITION + VERTEX_RGBA,
                GL_DYNAMIC_DRAW
            );
            
            
        }
        glfwPollEvents();
        glViewport(0,0, app.guiWindow.width, app.guiWindow.height);
        glClearColor(0,0,0,1);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindFramebuffer(GL_FRAMEBUFFER, app.frame.buffer);
        glViewport(0,0, 
            app.guiWindow.windowContent.appView.width, 
            app.guiWindow.windowContent.appView.height
        );
        glClearColor(0.5,0.5,0.5,1);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(app.flatShader);
        glUniformMatrix4fv(
            glGetUniformLocation(app.flatShader,"uProjMat"),
            1, GL_FALSE, (float*)app.camera.projMat
        );
        glUniformMatrix4fv(
            glGetUniformLocation(app.flatShader,"uViewMat"),
            1, GL_FALSE, (float*)app.camera.viewMat
        );
       
        {
            mat4x4 idMat;
            mat4x4_identity(idMat);
            glUniformMatrix4fv(
                glGetUniformLocation(app.flatShader,"uModelMat"),
                1, GL_FALSE, (float*)idMat
            );
        }
        model_draw(&app.testTriangle, VERTEX_POSITION + VERTEX_RGBA, MODEL_DRAW_VERTICIES, 3);
      
        if(app.floorGridModel.vertexCount){
            model_draw(&app.floorGridModel, VERTEX_POSITION + VERTEX_RGBA, 
                MODEL_DRAW_VERTICIES_LINES, 
                app.floorGridModel.vertexCount
            );
        }

        if(app.mainVoxels.head != NULL){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            model_draw(&app.voxelBox, VERTEX_POSITION + VERTEX_RGBA, 
                MODEL_DRAW_ELEMENTS, 
                ELEMENTS_PER_CUBE
            );    
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);     
        }

        if(app.cursor.isHit){
            model_draw(&app.hitModel, VERTEX_POSITION + VERTEX_RGBA, 
                MODEL_DRAW_ELEMENTS, 
                ELEMENTS_PER_CUBE
            );        
        }
        
        glUseProgram(app.phongShader);
        glUniformMatrix4fv(
            glGetUniformLocation(app.phongShader,"uProjMat"),
            1, GL_FALSE, (float*)app.camera.projMat
        );
        glUniformMatrix4fv(
            glGetUniformLocation(app.phongShader,"uViewMat"),
            1, GL_FALSE, (float*)app.camera.viewMat
        );
        {
            mat4x4 idMat;
            mat4x4_identity(idMat);
            glUniformMatrix4fv(
                glGetUniformLocation(app.phongShader,"uModelMat"),
                1, GL_FALSE, (float*)idMat
            );
        }
        glUniform3f(glGetUniformLocation(app.phongShader,"uDirectionLight"), 
            app.scene.lightDirection[0],
            app.scene.lightDirection[1],
            app.scene.lightDirection[2]
        );
        glUniform1f(glGetUniformLocation(app.phongShader,"uAmbientLight"), 0.2);
        glUniform1f(glGetUniformLocation(app.phongShader,"uLightIntensity"), 1);


        if(app.mainVoxels.head != NULL){
            model_draw(&app.voxelModel, VERTEX_POSITION + VERTEX_RGBA + VERTEX_NORMAL, 
                MODEL_DRAW_ELEMENTS, 
                ELEMENTS_PER_CUBE * app.mainVoxels.voxelCount
            );   
        }
        

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        nk_glfw3_new_frame(&glfw);

        nk_style_push_float(ctx, &ctx->style.window.padding.x, 0.0);
        nk_style_push_float(ctx, &ctx->style.window.padding.y, 0.0);
        nk_style_push_float(ctx, &ctx->style.window.spacing.x, 0.0);
        nk_style_push_float(ctx, &ctx->style.window.spacing.y, 0.0);
        nk_style_push_float(ctx, &ctx->style.window.group_padding.x, 0.0);
        nk_style_push_float(ctx, &ctx->style.window.group_padding.y, 0.0);
        if(nk_begin(ctx, "Gui Window", nk_rect(0,0, app.guiWindow.width, app.guiWindow.height),

            NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND | NK_WINDOW_BORDER
        )){
            nk_menubar_begin(ctx);
            nk_layout_row_template_begin(ctx, app.guiWindow.menuBar.height);
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

            nk_layout_row_template_begin(ctx, app.guiWindow.height - app.guiWindow.menuBar.height);
            nk_layout_row_template_push_static(ctx, app.guiWindow.windowContent.selectorTab.width);
            nk_layout_row_template_push_static(ctx, app.guiWindow.windowContent.propTab.width);
            nk_layout_row_template_push_static(ctx, app.guiWindow.windowContent.appView.width);
            nk_layout_row_template_end(ctx);
            
            if(nk_group_begin(ctx, "Property Selector", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
                nk_layout_row_static(ctx, 48, 48, 1);

                if(nk_select_label(ctx, "Cam", NK_TEXT_ALIGN_CENTERED, app.propertySelected == PROPERTY_CAMERA)){
                    app.propertySelected = PROPERTY_CAMERA;
                }
                if(nk_select_label(ctx, "Sce", NK_TEXT_ALIGN_CENTERED, app.propertySelected == PROPERTY_SCENE)){
                    app.propertySelected = PROPERTY_SCENE;
                }

                nk_group_end(ctx);
            }
            
            if(nk_group_begin(ctx, "Properties", NK_WINDOW_BORDER)){
                nk_layout_row_static(ctx, 32, 128, 1);

                switch (app.propertySelected)
                {
                case PROPERTY_CAMERA:
                    {
                        float *far = &app.camera.far;
                        nk_property_float(ctx,"far", 1, far, 360, 0.1, 0.1);
                        app.camera.far = *far;
                    }
                    {
                        float *fov = &app.camera.fov;
                        nk_property_float(ctx,"fov", -360, fov, 360, 0.1, 0.1);
                        app.camera.fov = *fov;
                    }
                    
                    {
                        float *distance = &app.camera.distance;
                        nk_property_float(ctx,"distance", 0.1, distance, 360, 0.1, 0.1);
                        app.camera.distance = *distance;
                    }

                    {
                        float *x = &app.camera.offset[0];
                        nk_property_float(ctx,"x", 0.1, x, 360, 0.1, 0.1);
                        app.camera.offset[0] = *x;
                    }
                    {
                        float *y = &app.camera.offset[1];
                        nk_property_float(ctx,"y", 0.1, y, 360, 0.1, 0.1);
                        app.camera.offset[1] = *y;
                    }
                    {
                        float *z = &app.camera.offset[2];
                        nk_property_float(ctx,"z", 0.1, z, 360, 0.1, 0.1);
                        app.camera.offset[2] = *z;
                    }
                    {
                        float *yaw = &app.camera.yaw;
                        nk_property_float(ctx,"yaw", -360, yaw, 360, 0.1, 0.1);
                        app.camera.yaw = *yaw;
                    }
                    {
                        float *pitch = &app.camera.pitch;
                        nk_property_float(ctx,"pitch", -360, pitch, 360, 0.1, 0.1);
                        app.camera.pitch = *pitch;
                    }
                    break;
                
                case PROPERTY_SCENE:
                    {
                        float *yaw = &app.scene.lightYaw;
                        nk_property_float(ctx,"yaw", -360, yaw, 360, 0.1, 0.1);
                        app.scene.lightYaw = *yaw;
                    }
                    {
                        float *pitch = &app.scene.lightPitch;
                        nk_property_float(ctx,"pitch", -360, pitch, 360, 0.1, 0.1);
                        app.scene.lightPitch = *pitch;
                    }

                    break;
                }
                nk_group_end(ctx);
                
            }

            if(nk_group_begin(ctx, "App View", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
                struct nk_image image = nk_image_id(app.frame.colorTexture);
                nk_layout_row_static(ctx, 
                    app.guiWindow.windowContent.appView.height,
                    app.guiWindow.windowContent.appView.width, 1
                );                
                nk_image_color(ctx, image, nk_white);
                nk_group_end(ctx);
            }
        }
        nk_end(ctx);
        if(nk_begin(ctx, "Toolbar", nk_rect(0,0, 64, 128), NK_WINDOW_MINIMIZABLE |
            NK_WINDOW_MOVABLE)){
                nk_layout_row_static(ctx, 32, 32, 2);
                if(nk_select_label(ctx, "Paint", NK_TEXT_ALIGN_LEFT, app.toolSelected == TOOL_PAINT)){
                    app.toolSelected = TOOL_PAINT;
                }
                if(nk_select_label(ctx, "Erase", NK_TEXT_ALIGN_LEFT, app.toolSelected == TOOL_ERASE)){
                    app.toolSelected = TOOL_ERASE;
                }



        }
        nk_end(ctx);

        nk_style_pop_float(ctx);
        nk_style_pop_float(ctx);
        nk_style_pop_float(ctx);
        nk_style_pop_float(ctx);
        nk_style_pop_float(ctx);
        nk_style_pop_float(ctx);
            
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
