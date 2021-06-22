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

int main(int argc, char const *argv[])
{    
    struct nk_glfw glfw = {0};
    struct nk_context *ctx;
    struct nk_font_atlas *atlas;
    GLFWwindow* window; 
    OrbitCamera orbitCamera;
    mat4x4 viewMat, projMat;
    GLuint gridShader;
    GridXZ gridY0;

   
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

    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    {struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    nk_glfw3_font_stash_end(&glfw);}

    {
    vec3 offset = {0.0,5.0,0.0};
    initOrbitCamera(&orbitCamera,offset,M_PI_4, M_PI_4,M_PI/3.0,1.0,100.0,1.0,10.0);
    mat4x4_perspective(projMat, orbitCamera.fov, orbitCamera.aspectRat, orbitCamera.near, orbitCamera.far);
    }

    initGridXZ(&gridY0, -16.0,-16.0,1.0,1.0,32,32);
    setGridXZColor(&gridY0,1.0,1.0,1.0);
    editGridXZModel(&gridY0);
    
    if(loadShaderProgram(&gridShader, "res/shaders/grid_vertex.glsl","res/shaders/grid_fragment.glsl")){
        printf("Failed to load shaders from these resource files\n");
        return 1;
    }

    
    while (!glfwWindowShouldClose(window)){
        float ratio;
        int width, height;
        quat camYaw, camPitch, camRot;
        vec3 up = {0,1,0};
        vec3 right = {1,0,0}; 

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        
        nk_glfw3_new_frame(&glfw);
        
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE ))
        {
            if (nk_button_label(ctx, "button")){
                fprintf(stdout, "button pressed\n");
            }
        }
        nk_end(ctx);
        
       
        if(orbitCamera.aspectRat != ratio){
            orbitCamera.aspectRat = ratio;
            mat4x4_perspective(projMat, orbitCamera.fov, orbitCamera.aspectRat, orbitCamera.near, orbitCamera.far);
        }

        {mat4x4 distMat, transMat, rotMat;
        quat_rotate(camYaw, orbitCamera.yaw, up);
        quat_rotate(camPitch, orbitCamera.pitch, right);
        quat_mul(camRot,camPitch, camYaw);
        mat4x4_translate(distMat, orbitCamera.distance, 0.0, 0.0);
        mat4x4_translate(transMat, -orbitCamera.offset[0], -orbitCamera.offset[1], -orbitCamera.offset[2]);
        
        
        mat4x4_from_quat(rotMat, camRot);
        mat4x4_mul(viewMat, rotMat, distMat);
        mat4x4_mul(viewMat, transMat, viewMat);
        }

        glViewport(0, 0, width, height);
        glClearColor(0.5, 0.5, 0.5, 0.5);
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
        glfwPollEvents();    
    }

    printf("hello world\n");
    nk_glfw3_shutdown(&glfw);
    return 0;
}
