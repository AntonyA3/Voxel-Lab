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


typedef struct Viewport
{
    int x, y, width, height;
    float ratio;
}Viewport;

typedef struct Panel{
    int x, y, width, height;
}Panel;
typedef struct Mouse
{
    float x, y, deltaX, deltaY;
    int leftState, rightState;
}Mouse;

typedef struct VoxelLab{
    int primaryAction, voxelEditFlag, navSelected;
    OrbitCamera orbitCam;
    mat4x4 viewMat, projMat;
    GLuint gridShader, voxelShader;
    GridXZ gridY0;
    Mouse mouse;
    Viewport mainViewport, renderViewport;
    VoxelStore voxelStore;
    VoxelVertex* voxelModelVerts;
    unsigned int* voxelModelIndicies;
    GLuint voxelModelVertexBuffer, voxelModelIndexBuffer;
    int voxelModelVertexCount, voxelModelIndexCount;
}VoxelLab;


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
/*Shader setup*/
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
void displayGridProperties(struct nk_context* ctx){
    //int gridVisible = !*visible;
    nk_layout_row_static(ctx, 32, 64, 1);
   // nk_checkbox_label(ctx, "Visible", &gridVisible);
    //*visible = !gridVisible;
    if(nk_tree_push(ctx, NK_TREE_TAB,"color", NK_MINIMIZED)){
        nk_layout_row_static(ctx, 32, 64, 2);
        
        nk_label(ctx,"r", NK_TEXT_LEFT);
        //gridColor->r =  nk_slide_int(ctx,0,gridColor->r*255,255,1)/(float)255;

        nk_label(ctx,"g", NK_TEXT_LEFT);
        //gridColor->g = nk_slide_int(ctx,0,gridColor->g*255,255,1)/(float)255;

        nk_label(ctx,"b", NK_TEXT_LEFT);
        //gridColor->b = nk_slide_int(ctx,0,gridColor->b*255,255,1)/(float)255;

        nk_label(ctx,"a", NK_TEXT_LEFT);
        //gridColor->a = nk_slide_int(ctx,0,gridColor->a*255,255,1)/(float)255;
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
void displayEditorProperties(struct nk_context* ctx){

}
void displayCameraProperties(struct nk_context* ctx){
    if(nk_tree_push(ctx, NK_TREE_TAB,"Projection", NK_MINIMIZED)){
        nk_layout_row_static(ctx, 32, 64, 2);

        //nk_label(ctx, "fov", NK_TEXT_ALIGN_LEFT);
        //camera->fov = nk_slide_float(ctx, degToRad(1.0f), camera->fov,degToRad(179.0f), degToRad(0.1f));
        //nk_label(ctx, "far", NK_TEXT_ALIGN_LEFT);

        //camera->far = nk_slide_float(ctx, 1.0, camera->far, 100, 0.01);
        nk_tree_pop(ctx);

    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    NEW_Y_SCROLL = yoffset;        
}
void updateFrameTexture(GLuint *texture, int width, int height){
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}
void updateFrameDepthTexture(GLuint *texture, int width, int height){
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width,height, 
        0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
    );
}

void mouseToViewportCoordinates(float* x, float* y, Viewport viewport, Mouse mouse){
    float halfWidth = viewport.width * 0.5;
    float halfHeight = viewport.height * 0.5;
    float midViewportX = viewport.x + halfWidth;
    float midViewportY = viewport.y + halfHeight;
    *x = (mouse.x - midViewportX) / halfWidth;
    *y = (mouse.y - midViewportY) / halfHeight * -1;

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


void mouseInit(GLFWwindow* window, Mouse* mouse){ 
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    mouse->x = (float)x;
    mouse->y = (float)y;
    mouse->deltaX = 0;
    mouse->deltaY = 0;
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

void initVoxelLab(VoxelLab *voxelLab){
    voxelLab->primaryAction = Default_Action;
    voxelLab->voxelEditFlag = voxel_edit_add_voxel;
    voxelLab->navSelected = nav_select_editor_props;
    voxelLab->mainViewport.x = 0;
    voxelLab->mainViewport.y = 0;

}

int main(int argc, char const *argv[])
{    
    struct nk_glfw glfw = {0};
    struct nk_context *ctx;
    VoxelLab voxelLab;
    GLFWwindow* window; 
    Panel menuPanel, navPanel, propPanel;

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
    
    initVoxelLab(&voxelLab);    
    glfwGetWindowSize(window, &voxelLab.mainViewport.width, &voxelLab.mainViewport.height);
    mouseInit(window, &voxelLab.mouse);

    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);
        nk_glfw3_font_stash_end(&glfw);
    }
    {
    vec3 offset = {0.0,0.0,0.0};
    initOrbitCamera(&voxelLab.orbitCam,offset,-M_PI_4, -M_PI_4,M_PI/3.0,1.0,10000.0,1.0,10.0);
    }

    initGridXZ(&voxelLab.gridY0, -16.0,-16.0,1.0,1.0,32,32, initGridColor(1.0,1.0,1.0,1.0), grid_visible_true);
    updateGridXZModel(&voxelLab.gridY0);
    
    if(loadShaderProgram(&voxelLab.gridShader, "res/shaders/grid_vertex.glsl","res/shaders/grid_fragment.glsl")){
        printf("nessesary shader failed to load\n");
        return 1;
    }
    if(loadShaderProgram(&voxelLab.voxelShader, "res/shaders/voxel_vertex.glsl", "res/shaders/voxel_fragment.glsl")){
        printf("nessessary shader failed to load\n");
        return 1;
    }

    GLuint frameBuffer, worldTexture, worldDepthTexture;    
    glGenTextures(1, &worldTexture);
    glGenTextures(1, &worldDepthTexture);
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    updateFrameTexture(&worldTexture, voxelLab.mainViewport.width, voxelLab.mainViewport.height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, worldTexture, 0);  
    updateFrameDepthTexture(&worldDepthTexture, voxelLab.mainViewport.width, voxelLab.mainViewport.height);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, worldDepthTexture, 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("Framebuffer is not complete!");
    }
 
    menuPanel.x = 0;
    menuPanel.y = 0;
    menuPanel.width = voxelLab.mainViewport.width;
    menuPanel.height = 32;
    
    propPanel.x = 64;
    propPanel.width = 144;
    propPanel.y = menuPanel.height;
    propPanel.height = voxelLab.mainViewport.height - menuPanel.height;

    glfwSetScrollCallback(window, scroll_callback);
    int count = 0;
    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 128; j++){
            for(int k = 0; k < 128; k++){
                addVoxelToVoxelStore(&voxelLab.voxelStore, i, j, k);

            }    
        }                 

    }
    compressVoxelStore(&voxelLab.voxelStore);

    getVoxelModelFromVoxelStore(&voxelLab.voxelStore, &voxelLab.voxelModelVerts, &voxelLab.voxelModelIndicies);  
    
    int voxelCount = countVoxelsInVoxelStore(&voxelLab.voxelStore);
    glGenBuffers(1, &voxelLab.voxelModelVertexBuffer);    
    glBindBuffer(GL_ARRAY_BUFFER, voxelLab.voxelModelVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(VoxelVertex) * 8 * voxelCount , voxelLab.voxelModelVerts, GL_DYNAMIC_DRAW);
    
    glGenBuffers(1, &voxelLab.voxelModelIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelLab.voxelModelIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36 * voxelCount, voxelLab.voxelModelIndicies, GL_DYNAMIC_DRAW);
    
    while (!glfwWindowShouldClose(window)){       
        int *primaryAction = &voxelLab.primaryAction;
        //int *voxelEditFlag = &voxelLab.voxelEditFlag;
        int *navSelected = &voxelLab.navSelected;

        
        OrbitCamera *orbitCam = &voxelLab.orbitCam;
        Mouse *mouse = &voxelLab.mouse;

        glfwGetFramebufferSize(window, &voxelLab.mainViewport.width, &voxelLab.mainViewport.height);
        voxelLab.mainViewport.ratio = voxelLab.mainViewport.width / (float) voxelLab.mainViewport.height;
        voxelLab.renderViewport.x = navPanel.width + propPanel.width;
        voxelLab.renderViewport.y = menuPanel.height;
        voxelLab.renderViewport.width =  voxelLab.mainViewport.width - navPanel.width - propPanel.width;
        voxelLab.renderViewport.height = voxelLab.mainViewport.height - navPanel.height;
        voxelLab.renderViewport.ratio = voxelLab.renderViewport.width / (float) voxelLab.renderViewport.height;
        voxelLab.orbitCam.aspectRat = voxelLab.renderViewport.ratio;
        getViewMat(&voxelLab.orbitCam, &voxelLab.viewMat);
        getProjMat(&voxelLab.orbitCam, &voxelLab.projMat);

        updateFrameTexture(&worldTexture, voxelLab.renderViewport.width, voxelLab.renderViewport.height);
        updateFrameDepthTexture(&worldDepthTexture, voxelLab.mainViewport.width, voxelLab.mainViewport.height);
        glfwPollEvents();    

        mouseUpdate(window, &voxelLab.mouse);
        voxelLab.orbitCam.distance += NEW_Y_SCROLL;

        mouse->rightState = glfwGetMouseButton(window,1);
        orbCamera(orbitCam, mouse->deltaX, mouse->deltaY, mouse->rightState);
        if (voxelLab.mouse.leftState == GLFW_PRESS)
        {
            switch (*primaryAction)
            {
            case Default_Action:
                break;
            case Pan_Camera:
                panCamera(orbitCam, voxelLab.mouse.deltaX, voxelLab.mouse.deltaY);
                break;
            case Rotate_Camera:
                orbCamera(orbitCam, mouse->deltaX, mouse->deltaY, mouse->leftState);
                break;
            case Zoom_Camera:
                voxelLab.orbitCam.distance += voxelLab.mouse.deltaY;
                break;            
            default:
                break;
            }    
        }
        if(voxelLab.mouse.leftState == GLFW_RELEASE){
            *primaryAction = Default_Action;
        }

        nk_glfw3_new_frame(&glfw);
        
        if(nk_begin(ctx, "Main Panel", nk_rect(0,0, voxelLab.mainViewport.width, voxelLab.mainViewport.height),NK_WINDOW_BORDER | NK_WINDOW_BACKGROUND | NK_WINDOW_NO_SCROLLBAR)){
            nk_layout_row_static(ctx, menuPanel.height, 48, 4);
            infoMenu(ctx);
            fileMenu(window, ctx);
            editMenu(ctx);
            aboutMenu(ctx);
            
            nk_layout_set_min_row_height(ctx,voxelLab.renderViewport.height);
            nk_layout_row_begin(ctx, NK_STATIC, 0, 3);
            nk_layout_row_push(ctx, 48.0f);
            nk_layout_reset_min_row_height(ctx);
            float containerWidth = nk_widget_width(ctx);
            if(nk_group_begin(ctx, "Prop Type Container",NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
                nk_layout_row_static(ctx,containerWidth - 8 ,containerWidth - 8,1);
                if(nk_select_label(ctx, "Edit",NK_TEXT_ALIGN_CENTERED, 
                    (*navSelected == nav_select_voxel_edit) ? nk_true: nk_false)){
                        *navSelected = nav_select_voxel_edit;
                }
                if(nk_select_label(ctx, "Camera",NK_TEXT_ALIGN_CENTERED, 
                    (*navSelected == nav_select_camera_props) ? nk_true: nk_false)){
                        *navSelected = nav_select_camera_props;
                }
                if(nk_select_label(ctx, "Editor",NK_TEXT_ALIGN_CENTERED, 
                    (*navSelected == nav_select_editor_props) ? nk_true: nk_false)){
                        *navSelected = nav_select_editor_props;
                }

                if(nk_select_label(ctx, "Grid",NK_TEXT_ALIGN_CENTERED, 
                    (*navSelected == nav_select_grid_props) ? nk_true: nk_false)){
                        *navSelected = nav_select_grid_props;
                }
                nk_group_end(ctx);
            }
            
            nk_layout_row_push(ctx, propPanel.width);
            
            if(nk_group_begin(ctx, "Property View",NK_WINDOW_BORDER)){
                switch (*navSelected){
                    case nav_select_voxel_edit:
                        displayVoxelEditor(ctx, &voxelLab.voxelEditFlag);
                        break;
                    case nav_select_camera_props:   
                        displayCameraProperties(ctx);
                        break;
                    case nav_select_editor_props:
                        displayEditorProperties(ctx);
                        break;
                    case nav_select_grid_props:
                        displayGridProperties(ctx);
                        break;
                }
                nk_group_end(ctx);
            }
            

            nk_layout_row_push(ctx, voxelLab.renderViewport.width);
            if(nk_group_begin(ctx, "Canvas View",NK_WINDOW_BORDER)){                
                nk_layout_row_static(ctx, voxelLab.renderViewport.height, voxelLab.renderViewport.width,1);
                nk_image_color(ctx, nk_image_id((int)worldTexture),nk_white);
                nk_group_end(ctx);

            }
        }
        nk_end(ctx);     

        struct nk_color transparent = {0x00, 0x00, 0x00, 0x00};
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_color(transparent));
        nk_style_push_color(ctx,&ctx->style.window.background, transparent);
        if(nk_begin(ctx, "Cam Button Panel", nk_rect(voxelLab.mainViewport.width - 96,128, 64, 64*4), NK_WINDOW_NO_SCROLLBAR)){
            nk_layout_row_static(ctx, 32, 32,1 );
        
            nk_style_push_float(ctx, &ctx->style.selectable.rounding, 15);
            nk_bool down = nk_widget_has_mouse_click_down(ctx, NK_BUTTON_LEFT, nk_true);
            if(down){
                nk_select_label(ctx, "r",NK_TEXT_ALIGN_CENTERED, nk_true);
                *primaryAction = Rotate_Camera;
            }else{
                nk_select_label(ctx, "r",NK_TEXT_ALIGN_CENTERED, nk_false);
            }
            down = nk_widget_has_mouse_click_down(ctx, NK_BUTTON_LEFT, nk_true);
            if(down){
                nk_select_label(ctx, "p",NK_TEXT_ALIGN_CENTERED, nk_true);
                *primaryAction = Pan_Camera;
            }else{
                nk_select_label(ctx, "p",NK_TEXT_ALIGN_CENTERED, nk_false);
            }
            down = nk_widget_has_mouse_click_down(ctx, NK_BUTTON_LEFT, nk_true);
            if(down){
                nk_select_label(ctx, "z",NK_TEXT_ALIGN_CENTERED, nk_true);
                *primaryAction = Zoom_Camera;
            }else{
                nk_select_label(ctx, "z",NK_TEXT_ALIGN_CENTERED, nk_false);
            }
            nk_style_pop_float(ctx);
        }
        nk_end(ctx);
        nk_style_pop_color(ctx);
        nk_style_pop_style_item(ctx);

        

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        //glCullFace(GL_BACK);
        glViewport(0, 0, voxelLab.renderViewport.width, voxelLab.renderViewport.height);
        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
        glBindBuffer(GL_ARRAY_BUFFER, voxelLab.voxelModelVertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, voxelLab.voxelModelIndexBuffer);
        
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3* sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glUseProgram(voxelLab.voxelShader);
        glUniformMatrix4fv(glGetUniformLocation(voxelLab.voxelShader,"uProjMat"),1,GL_FALSE, (float*)*voxelLab.projMat);
        glUniformMatrix4fv(glGetUniformLocation(voxelLab.voxelShader,"uViewMat"),1,GL_FALSE, (float*)*voxelLab.viewMat);
        glDrawElements(GL_TRIANGLES, 36 * countVoxelsInVoxelStore(&voxelLab.voxelStore), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if(voxelLab.gridY0.visible){             
            glBindBuffer(GL_ARRAY_BUFFER, voxelLab.gridY0.model.vertsBuffer);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7* sizeof(float),(void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE,7 * sizeof(float),(void*)(sizeof(float) * 3));
            glEnableVertexAttribArray(1);
            glUseProgram(voxelLab.gridShader);
            glUniformMatrix4fv(glGetUniformLocation(voxelLab.gridShader,"uProjMat"),1,GL_FALSE, (float*)*voxelLab.projMat);
            glUniformMatrix4fv(glGetUniformLocation(voxelLab.gridShader,"uViewMat"),1,GL_FALSE, (float*)*voxelLab.viewMat);
            glDrawArrays(GL_LINES,0, gridXZVertexCount(voxelLab.gridY0));
        } 
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(window);
        NEW_Y_SCROLL = 0;
    }

    nk_glfw3_shutdown(&glfw);
    return 0;
}
