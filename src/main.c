#include <stdio.h>
#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>
#include "../include/camera.h"
#include "../include/grid.h"
#define M_PI 3.14159265358979323846


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
    
    orbitCamera.fov = M_PI/3.0;
    orbitCamera.near = 1.0;
    orbitCamera.far = 100.0;
    orbitCamera.distance = 10.0;
    orbitCamera.yaw = M_PI_4;
    orbitCamera.pitch = M_PI_4;
    orbitCamera.aspectRat = 1.0;
    orbitCamera.offset[0] = 0.0;
    orbitCamera.offset[1] = 5.0;
    orbitCamera.offset[2] = 0.0;
    mat4x4_perspective(projMat, orbitCamera.fov, orbitCamera.aspectRat, orbitCamera.near, orbitCamera.far);


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

        if(orbitCamera.aspectRat != ratio){
            orbitCamera.aspectRat = ratio;
            mat4x4_perspective(projMat, orbitCamera.fov, orbitCamera.aspectRat, orbitCamera.near, orbitCamera.far);
        }        
        {
            mat4x4 distMat, transMat, rotMat;
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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    printf("hello world\n");
    
    return 0;
}
