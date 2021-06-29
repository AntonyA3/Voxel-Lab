#include <stdio.h>
#define M_PI 3.14159265358979323846
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
#include "../include/nuklear.h"
#include "../include/nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#include "../include/mouse_input.h"
#include "../include/camera.h"
#include "../include/grid.h"
#include "../include/voxel.h"
float NEW_Y_SCROLL = 0;


typedef struct viewport
{
    int x, y, width, height;
    float ratio;
}viewport;


typedef struct Panel{
    int x, y, width, height;
}Panel;
typedef struct Mouse
{
    float x, y, deltaX, deltaY;
    int leftState, rightState;
}Mouse;

typedef struct SceneBackCol
{
    float r, g, b, a;
}SceneBackCol;

typedef struct Scene
{
    SceneBackCol backCol;
}Scene;

enum mouseLeftEvt{Default_Action,Pan_Camera, Rotate_Camera, Zoom_Camera};
enum voxel_edit{voxel_edit_add_voxel, voxel_edit_delete_voxel};
enum nav_select{nav_select_voxel_edit, nav_select_camera_props, nav_select_editor_props, nav_select_grid_props};

const float quad[6][4] = {
    {-1.0,-1.0,0.0,0.0},
    {-1.0,1.0,0.0,1.0},
    {1.0,1.0,1.0,1.0},
    
    {-1.0,-1.0,0.0,0.0},
    {1.0,1.0,1.0,1.0},
    {1.0,-1.0,1.0,0.0}
};
int createShaderProgram(GLuint *program, const char *vertShadeSrc, const char *fragShadeSrc){
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
int loadTextFile(const char* path, const char** response){
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
int loadShaderProgram(GLuint* program,const char* vertexPath,const char* fragmentPath){
    const char* vertexShaderSrc = NULL;
    const char* fragmentShaderSrc = NULL;
    if(loadTextFile(vertexPath, &vertexShaderSrc)){
        printf("failed to load vertex shader file\n");
        return 1;
    }

    if(loadTextFile(fragmentPath, &fragmentShaderSrc)){
        printf("failed to load fragment shader file\n");
        return 1;
    }
    
    if(createShaderProgram(program, vertexShaderSrc, fragmentShaderSrc)){
        printf("failed to create shader program\n");
        return 1;
    }
    return 0;
}
float degToRad(float deg){
    return deg /180 * M_PI;
}
void displayGridProperties(struct nk_context* ctx, int* visible, GridColor* gridColor){
    int gridVisible = !*visible;
    nk_layout_row_static(ctx, 32, 64, 1);
    nk_checkbox_label(ctx, "Visible", &gridVisible);
    *visible = !gridVisible;
    if(nk_tree_push(ctx, NK_TREE_TAB,"color", NK_MINIMIZED)){
        nk_layout_row_static(ctx, 32, 64, 2);
        
        nk_label(ctx,"r", NK_TEXT_LEFT);
        gridColor->r =  nk_slide_int(ctx,0,gridColor->r*255,255,1)/(float)255;

        nk_label(ctx,"g", NK_TEXT_LEFT);
        gridColor->g = nk_slide_int(ctx,0,gridColor->g*255,255,1)/(float)255;

        nk_label(ctx,"b", NK_TEXT_LEFT);
        gridColor->b = nk_slide_int(ctx,0,gridColor->b*255,255,1)/(float)255;

        nk_label(ctx,"a", NK_TEXT_LEFT);
        gridColor->a = nk_slide_int(ctx,0,gridColor->a*255,255,1)/(float)255;
        nk_tree_pop(ctx);
    }

}

void displayVoxelEditor(struct nk_context* ctx, int* action){
    nk_layout_row_static(ctx, 32, 32, 3);

    if(nk_select_label(ctx, "Add Voxel", NK_TEXT_LEFT, 
    (*action == voxel_edit_add_voxel) ? nk_true : nk_false)){
        *action = voxel_edit_add_voxel;
    }

    if(nk_select_label(ctx, "Delete Voxel", NK_TEXT_LEFT,
    (*action == voxel_edit_delete_voxel) ? nk_true : nk_false)){
        *action = voxel_edit_delete_voxel;
    }
}
void displayEditorProperties(struct nk_context* ctx){}
void displayCameraProperties(struct nk_context* ctx, float* fov, float* far){
    if(nk_tree_push(ctx, NK_TREE_TAB,"Projection", NK_MINIMIZED)){
        nk_layout_row_static(ctx, 32, 64, 2);

        nk_label(ctx, "fov", NK_TEXT_ALIGN_LEFT);
        *fov = nk_slide_float(ctx, degToRad(1.0f),*fov,degToRad(179.0f), degToRad(0.1f));
        nk_label(ctx, "far", NK_TEXT_ALIGN_LEFT);

        *far = nk_slide_float(ctx, 1.0, *far, 100, 0.01);
        nk_tree_pop(ctx);

    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    NEW_Y_SCROLL = yoffset;        
}
void updateFrameTexture(GLuint* texture, int width, int height){
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);


}
void mouseToViewportCoordinates(float* x, float* y, viewport viewport, Mouse mouse){
    float halfWidth = viewport.width * 0.5;
    float halfHeight = viewport.height * 0.5;
    float midViewportX = viewport.x + halfWidth;
    float midViewportY = viewport.y + halfHeight;
    *x = (mouse.x - midViewportX) / halfWidth;
    *y = (mouse.y - midViewportY) / halfHeight * -1;

}
void editVoxels(int voxelEditFlag, Mouse mouse, viewport viewport, mat4x4 projMat, mat4x4 viewMat, vec3 camPos){
    switch (voxelEditFlag)
    {
    case voxel_edit_add_voxel:
        addVoxel(mouse, viewport, projMat, viewMat, camPos);
        break;
    case voxel_edit_delete_voxel:
        break;
    }
}

void addVoxelFromRay(vec3 start, vec3 direction){
    if((start[1] > 0 && direction[1] < 0) || (start[1] < 0 && direction[1] > 0)){
        float d = (0 - start[1]) / direction[1];
        int x = (int) (start[0] + direction[0] * d);
        int y = 0;
        int z = (int) (start[2] + direction[2] * d);
        printf("%d, %d, %d\n",x,y,z);
    }

}
void addVoxel(Mouse mouse, viewport viewport, mat4x4 projMat, mat4x4 viewMat, vec3 camPos){
    float x, y;
    vec4 direction;
    mat4x4 mulMat, inverseMat;
    mouseToViewportCoordinates(&x, &y, viewport,mouse);
    vec4 clickPoint = {x, y, -1.0, 1.0};
    
    mat4x4_mul(mulMat, projMat, viewMat);
    mat4x4_invert(inverseMat,mulMat);
    mat4x4_mul_vec4(direction, inverseMat,clickPoint);

    float divizor = -1 /direction[3];
    vec3 direction3 = {direction[0] * divizor, direction[1] * divizor, direction[2] * divizor};
    vec3_norm(direction3, direction3);
    addVoxelFromRay(camPos, direction3);
}


void deleteVoxel(){

}
void infoMenu(struct  nk_context* ctx)
{
    struct nk_vec2 size = {64,24*5};
    if(nk_menu_begin_label(ctx,"info", NK_TEXT_CENTERED, size)){
        nk_layout_row_dynamic(ctx, 25,1);
        nk_menu_end(ctx);
    }
};
void fileMenu(GLFWwindow* window,struct nk_context* ctx){
    struct nk_vec2 size = {64,24*5};
    if(nk_menu_begin_label(ctx, "File",NK_TEXT_CENTERED, size)){
        nk_layout_row_dynamic(ctx, 25,1);
        nk_menu_item_label(ctx, "New", NK_TEXT_LEFT);
        nk_menu_item_label(ctx, "Open", NK_TEXT_LEFT);
        nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT);
        if(nk_menu_item_label(ctx, "Quit", NK_TEXT_LEFT)){
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        nk_menu_end(ctx);
    }

}
void editMenu(struct nk_context* ctx){
    struct nk_vec2 size = {64,24*5};
    if(nk_menu_begin_label(ctx,"Edit", NK_TEXT_CENTERED, size)){
        nk_layout_row_dynamic(ctx, 25,1);
        nk_menu_end(ctx);
    }
}
void aboutMenu(struct nk_context* ctx){
    struct nk_vec2 size = {128,24*5};
    if(nk_menu_begin_label(ctx, "About",NK_TEXT_CENTERED, size)){
        nk_layout_row_dynamic(ctx, 25,1);
        nk_menu_item_label(ctx, "Contributors", NK_TEXT_LEFT);
        nk_menu_item_label(ctx, "Documentation", NK_TEXT_LEFT);
        nk_menu_end(ctx);
    }

}

void renderScene(Scene scene,int gridVisible, GLuint target, int width, int height, 
    GridXZ grid, GLuint gridShader, mat4x4* projMat, mat4x4* viewMat){
    float* backCol = &scene.backCol;
    if(gridVisible){
        glBindFramebuffer(GL_FRAMEBUFFER, target);
        glViewport(0, 0, width, height);
        glClearColor(backCol[0], backCol[1], backCol[2], backCol[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       
        glBindBuffer(GL_ARRAY_BUFFER, grid.model.vertsBuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7* sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE,7 * sizeof(float),(void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glUseProgram(gridShader);
        glUniformMatrix4fv(glGetUniformLocation(gridShader,"uProjMat"),1,GL_FALSE,(float*)*projMat);
        glUniformMatrix4fv(glGetUniformLocation(gridShader,"uViewMat"),1,GL_FALSE,(float*)*viewMat);
        glDrawArrays(GL_LINES,0, gridXZVertexCount(grid));
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


    }
}

void mouseInit(GLFWwindow* window, Mouse mouse){ 
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    mouse.x = (float)x;
    mouse.y = (float)y;
    mouse.deltaX = 0;
    mouse.deltaY = 0;
}

void mouseUpdate(GLFWwindow* window, Mouse* mouse){
    double nMousePosX, nMousePosY;
    mouse->leftState = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT);
    mouse->rightState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    glfwGetCursorPos(window, &nMousePosX, &nMousePosY);
    mouse->deltaX = (float) (nMousePosX - mouse->x);
    mouse->deltaY = (float) (nMousePosY - mouse->y);
    mouse->x = nMousePosX;
    mouse->y = nMousePosY;
}


int main(int argc, char const *argv[])
{    
    struct nk_glfw glfw = {0};
    struct nk_context *ctx;
    int primaryAction = Default_Action;
    int voxelEditFlag = voxel_edit_add_voxel;
    int navSelected = nav_select_editor_props;
    GLFWwindow* window; 
    OrbitCamera orbitCamera;
    mat4x4 viewMat, projMat;
    GLuint gridShader;
    GridXZ gridY0;
    Mouse mouse;
    Scene scene;
    viewport mainViewport, renderViewport;
    Panel menuPanel, navPanel, propPanel, renderPanel;
    VoxelStore voxelStore;
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
    mainViewport.x = mainViewport.y = 0;
    glfwGetWindowSize(window, &mainViewport.width, &mainViewport.height);

    mouseInit(window, mouse);

    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    {struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    nk_glfw3_font_stash_end(&glfw);}

    {
    vec3 offset = {0.0,0.0,0.0};
    initOrbitCamera(&orbitCamera,offset,-M_PI_4, -M_PI_4,M_PI/3.0,1.0,100.0,1.0,10.0);
    }

    initGridXZ(&gridY0, -16.0,-16.0,1.0,1.0,32,32, initGridColor(1.0,1.0,1.0,1.0), grid_visible_true);
    updateGridXZModel(&gridY0);
    
    if(loadShaderProgram(&gridShader, "res/shaders/grid_vertex.glsl","res/shaders/grid_fragment.glsl")){
        printf("Failed to load shaders from these resource files\n");
        return 1;
    }
    {
        float grey[4] = {0.5,0.5,0.5,1.0};
        memcpy(&scene.backCol, &grey, sizeof(SceneBackCol));
    }

    GLuint frameBuffer, worldTexture;    
    glGenTextures(1, &worldTexture);
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    updateFrameTexture(&worldTexture, mainViewport.width, mainViewport.height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, worldTexture, 0);  
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("Framebuffer is not complete!");
    }
 
    menuPanel.x = 0;
    menuPanel.y = 0;
    menuPanel.width = mainViewport.width;
    menuPanel.height = 32;
    

    propPanel.x = 64;
    propPanel.width = 144;
    propPanel.y = menuPanel.height;
    propPanel.height = mainViewport.height - menuPanel.height;

    glfwSetScrollCallback(window, scroll_callback);

    while (!glfwWindowShouldClose(window)){        
        glfwGetFramebufferSize(window, &mainViewport.width, &mainViewport.height);
        mainViewport.ratio = mainViewport.width / (float) mainViewport.height;
        renderViewport.x = navPanel.width + propPanel.width;
        renderViewport.y = menuPanel.height;
        renderViewport.width =  mainViewport.width - navPanel.width - propPanel.width;
        renderViewport.height = mainViewport.height - navPanel.height;
        renderViewport.ratio = renderViewport.width / (float) renderViewport.height;
        orbitCamera.aspectRat = renderViewport.ratio;
        getViewMat(&orbitCamera, &viewMat);
        getProjMat(&orbitCamera, &projMat);

        updateFrameTexture(&worldTexture, renderViewport.width, renderViewport.height);

        glfwPollEvents();    
        mouseUpdate(window, &mouse);
        orbitCamera.distance += NEW_Y_SCROLL;
        vec3 camPos;

        if (mouse.leftState == GLFW_PRESS)
        {
            switch (primaryAction)
            {
            case Default_Action:
                orbitCameraPosition(orbitCamera, camPos);
                editVoxels(voxelEditFlag, mouse, renderViewport, projMat, viewMat,camPos);
                break;
            case Pan_Camera:
                panCamera(&orbitCamera, mouse.deltaX, mouse.deltaY);
                break;
            case Rotate_Camera:
                cameraOrbitEvent(&orbitCamera, mouse.leftState ,mouse.deltaX, mouse.deltaY);
                break;
            case Zoom_Camera:
                orbitCamera.distance += mouse.deltaY;
                break;            
            default:
                break;
            }    
        }
        if(mouse.leftState == GLFW_RELEASE){
            primaryAction = Default_Action;
        }
        cameraOrbitEvent(&orbitCamera, mouse.rightState ,mouse.deltaX, mouse.deltaY);


        nk_glfw3_new_frame(&glfw);
        
        if(nk_begin(ctx, "Main Panel", nk_rect(0,0, mainViewport.width, mainViewport.height),NK_WINDOW_BORDER | NK_WINDOW_BACKGROUND | NK_WINDOW_NO_SCROLLBAR)){
            nk_layout_row_static(ctx, menuPanel.height, 48, 4);
            infoMenu(ctx);
            fileMenu(window, ctx);
            editMenu(ctx);
            aboutMenu(ctx);
            
            nk_layout_set_min_row_height(ctx,renderViewport.height);
            nk_layout_row_begin(ctx, NK_STATIC, 0, 3);
            nk_layout_row_push(ctx, 48.0f);
            nk_layout_reset_min_row_height(ctx);

            float containerWidth = nk_widget_width(ctx);
            if(nk_group_begin(ctx, "Prop Type Container",NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
                nk_layout_row_static(ctx,containerWidth - 8 ,containerWidth - 8,1);
                
                if(nk_select_label(ctx, "Edit",NK_TEXT_ALIGN_CENTERED, 
                    (navSelected == nav_select_voxel_edit) ? nk_true: nk_false)){
                        navSelected = nav_select_voxel_edit;
                }
                if(nk_select_label(ctx, "Camera",NK_TEXT_ALIGN_CENTERED, 
                    (navSelected == nav_select_camera_props) ? nk_true: nk_false)){
                        navSelected = nav_select_camera_props;
                }
                if(nk_select_label(ctx, "Editor",NK_TEXT_ALIGN_CENTERED, 
                    (navSelected == nav_select_editor_props) ? nk_true: nk_false)){
                        navSelected = nav_select_editor_props;
                }

                if(nk_select_label(ctx, "Grid",NK_TEXT_ALIGN_CENTERED, 
                    (navSelected == nav_select_grid_props) ? nk_true: nk_false)){
                        navSelected = nav_select_grid_props;
                }
                nk_group_end(ctx);
            }
            
            nk_layout_row_push(ctx, propPanel.width);
            
            if(nk_group_begin(ctx, "Property View",NK_WINDOW_BORDER)){
                switch (navSelected){
                case nav_select_voxel_edit:
                    displayVoxelEditor(ctx, &voxelEditFlag);
                    break;
                case nav_select_camera_props:   
                    displayCameraProperties(ctx, &orbitCamera.fov, &orbitCamera.far);
                    break;
                case nav_select_editor_props:
                    displayEditorProperties(ctx);
                    break;
                case nav_select_grid_props:
                    displayGridProperties(ctx, &gridY0.visible, &gridY0.color);
                    break;
                }
                nk_group_end(ctx);
            }
            

            nk_layout_row_push(ctx, renderViewport.width);
            if(nk_group_begin(ctx, "Canvas View",NK_WINDOW_BORDER)){                
                nk_layout_row_static(ctx, renderViewport.height, renderViewport.width,1);
                nk_image_color(ctx, nk_image_id((int)worldTexture),nk_white);
                nk_group_end(ctx);

            }
        }
        nk_end(ctx);     

        struct nk_color transparent = {0x00, 0x00, 0x00, 0x00};
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_color(transparent));
        nk_style_push_color(ctx,&ctx->style.window.background, transparent);
        if(nk_begin(ctx, "Cam Button Panel", nk_rect(mainViewport.width - 96,128, 64, 64*4), NK_WINDOW_NO_SCROLLBAR)){
            nk_layout_row_static(ctx, 32, 32,1 );
        
            nk_style_push_float(ctx, &ctx->style.selectable.rounding, 15);
            nk_bool down = nk_widget_has_mouse_click_down(ctx, NK_BUTTON_LEFT, nk_true);
            if(down){
                nk_select_label(ctx, "r",NK_TEXT_ALIGN_CENTERED, nk_true);
                primaryAction = Rotate_Camera;
            }else{
                nk_select_label(ctx, "r",NK_TEXT_ALIGN_CENTERED, nk_false);
            }
            down = nk_widget_has_mouse_click_down(ctx, NK_BUTTON_LEFT, nk_true);
            if(down){
                nk_select_label(ctx, "p",NK_TEXT_ALIGN_CENTERED, nk_true);
                primaryAction = Pan_Camera;
            }else{
                nk_select_label(ctx, "p",NK_TEXT_ALIGN_CENTERED, nk_false);
            }
            down = nk_widget_has_mouse_click_down(ctx, NK_BUTTON_LEFT, nk_true);
            if(down){
                nk_select_label(ctx, "z",NK_TEXT_ALIGN_CENTERED, nk_true);
                primaryAction = Zoom_Camera;
            }else{
                nk_select_label(ctx, "z",NK_TEXT_ALIGN_CENTERED, nk_false);
            }
            nk_style_pop_float(ctx);
        }
        nk_end(ctx);
        nk_style_pop_color(ctx);
        nk_style_pop_style_item(ctx);

        
        renderScene(scene, gridY0.visible, frameBuffer,renderViewport.width, renderViewport.height, gridY0, gridShader, &projMat, &viewMat);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(window);
        NEW_Y_SCROLL = 0;
    }

    nk_glfw3_shutdown(&glfw);
    return 0;
}
