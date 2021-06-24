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

#include "../include/camera.h"
#include "../include/grid.h"
/*To create a globals struct*/
float NEW_Y_SCROLL = 0;

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    NEW_Y_SCROLL = yoffset;        
}


int main(int argc, char const *argv[])
{    
    struct nk_glfw glfw = {0};
    struct nk_context *ctx;
    struct nk_font_atlas *atlas;   
    enum bool{false, true};

    enum mouseLeftEvt{Default_Action,Pan_Camera, Rotate_Camera, Zoom_Camera};
    int activeMouseLeftEvent = Default_Action;
    GLFWwindow* window; 
    OrbitCamera orbitCamera;
    int isCameraOrbit = false;

    mat4x4 viewMat, projMat;
    GLuint gridShader;
    GridXZ gridY0;
    float glBackgroundCol[4] = {0.5,0.5,0.5,1.0};
    double mousePosX, mousePosY;

   
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
    
    glfwGetCursorPos(window, &mousePosX, &mousePosY);

    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    {struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    nk_glfw3_font_stash_end(&glfw);}

    {
    vec3 offset = {0.0,0.0,0.0};
    initOrbitCamera(&orbitCamera,offset,-M_PI_4, -M_PI_4,M_PI/3.0,1.0,100.0,1.0,10.0);
    }

    initGridXZ(&gridY0, -16.0,-16.0,1.0,1.0,32,32);
    setGridXZColor(&gridY0,1.0,1.0,1.0);
    editGridXZModel(&gridY0);
    
    if(loadShaderProgram(&gridShader, "res/shaders/grid_vertex.glsl","res/shaders/grid_fragment.glsl")){
        printf("Failed to load shaders from these resource files\n");
        return 1;
    }
    /*
    GLuint frameBuffer, worldTexture;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &worldTexture);
    glBindTexture(GL_TEXTURE_2D, worldTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, worldTexture, 0);  
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  */
    glfwSetScrollCallback(window, scroll_callback);

    while (!glfwWindowShouldClose(window)){
        float ratio;
        int width, height;
        double nMousePosX, nMousePosY;
        float dMousePosX, dMousePosY;
        
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);


        glfwPollEvents();    
        int leftBtnState = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT);
        int rightBtnState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        glfwGetCursorPos(window, &nMousePosX, &nMousePosY);
        dMousePosX = (float) (nMousePosX - mousePosX);
        dMousePosY = (float) (nMousePosY - mousePosY);
        mousePosX = nMousePosX;
        mousePosY = nMousePosY;
        orbitCamera.distance += NEW_Y_SCROLL;
        if (leftBtnState == GLFW_PRESS)
        {
            switch (activeMouseLeftEvent)
            {
            case Default_Action:
                break;
            case Pan_Camera:
                panCamera(&orbitCamera, dMousePosX, dMousePosY);
                break;
            case Rotate_Camera:
                cameraOrbitEvent(&orbitCamera, &isCameraOrbit, leftBtnState ,dMousePosX, dMousePosY);

                break;
            case Zoom_Camera:
                orbitCamera.distance += dMousePosY;
                break;
            
            default:
                break;
            }
            
        }
        if(leftBtnState == GLFW_RELEASE){
            activeMouseLeftEvent = Default_Action;
        }
        if(activeMouseLeftEvent != Rotate_Camera){
            cameraOrbitEvent(&orbitCamera, &isCameraOrbit, rightBtnState ,dMousePosX, dMousePosY);
        }


        nk_glfw3_new_frame(&glfw);
        
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE ))
        {
            nk_layout_row_static(ctx, 30, 80, 1);
            orbitCamera.fov = nk_slide_float(ctx,0.01, orbitCamera.fov, M_PI,0.01);
            orbitCamera.far = nk_slide_float(ctx, orbitCamera.near, orbitCamera.far, 200, 0.01);
            getProjMat(&orbitCamera, &projMat);

            nk_layout_row_static(ctx, 32, 32, 1);
            nk_style_push_float(ctx, &ctx->style.button.rounding, 16);

            if(nk_button_symbol(ctx, NK_SYMBOL_NONE)){                
                activeMouseLeftEvent = Zoom_Camera;
            }
            
            if(nk_button_symbol(ctx, NK_SYMBOL_NONE)){                
                activeMouseLeftEvent = Pan_Camera;
            }

            if(nk_button_symbol(ctx, NK_SYMBOL_NONE)){                
                activeMouseLeftEvent = Rotate_Camera;
            }
           
            nk_style_pop_float(ctx);
            nk_button_label(ctx, "button");

        }
        nk_end(ctx);

        if (nk_begin(ctx, "Bacground Color Picker", nk_rect(100, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE ))
        {
            
            struct nk_colorf bgcol;
            memcpy(&bgcol,glBackgroundCol, sizeof(struct nk_colorf));
            nk_layout_row_static(ctx, 230, 250, 1);
            bgcol = nk_color_picker(ctx,bgcol,NK_RGBA);
            memcpy(glBackgroundCol,&bgcol,sizeof(struct nk_colorf));
            
        
        }
        nk_end(ctx);

        if (nk_begin(ctx, "Grid Color Picker", nk_rect(100, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE ))
        {
            //nk_image_id()
            //nk_draw_image()
            //float gridColor[4] = {gridY0.r,gridY0.g, gridY0.b, gridY0.a};
            struct nk_colorf bgcol;
            memcpy(&bgcol,&gridY0.r, sizeof(struct nk_colorf));
            nk_layout_row_static(ctx, 230, 250, 1);
            bgcol = nk_color_picker(ctx,bgcol,NK_RGBA);
            memcpy(&gridY0.r,&bgcol,sizeof(struct nk_colorf));
            editGridXZModel(&gridY0);
        
        }
        nk_end(ctx);

        if(orbitCamera.aspectRat != ratio){
            orbitCamera.aspectRat = ratio;
        }

        getViewMat(&orbitCamera, &viewMat);
        getProjMat(&orbitCamera, &projMat);

        glClearColor(glBackgroundCol[0], glBackgroundCol[1], glBackgroundCol[2], glBackgroundCol[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindBuffer(GL_ARRAY_BUFFER, gridY0.model.vertsBuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7* sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE,7 * sizeof(float),(void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        
        glUseProgram(gridShader);
        glUniformMatrix4fv(glGetUniformLocation(gridShader,"uProjMat"),1,GL_FALSE,(float*)&projMat);
        glUniformMatrix4fv(glGetUniformLocation(gridShader,"uViewMat"),1,GL_FALSE,(float*)&viewMat);
        glDrawArrays(GL_LINES,0, gridXZVertexCount(gridY0));
        
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

        glfwSwapBuffers(window);
        NEW_Y_SCROLL = 0;
    }

    printf("hello world\n");
    nk_glfw3_shutdown(&glfw);
    return 0;
}
