#include "../include/shader_resource.h"

int create_shader_program(GLuint *program, const char *vertShadeSrc, const char *fragShadeSrc){
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
int load_text_file(const char* path, const char** response){
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
int load_shader_program(GLuint* program,const char* vertexPath,const char* fragmentPath){
    const char* vertexShaderSrc = NULL;
    const char* fragmentShaderSrc = NULL;
    if(load_text_file(vertexPath, &vertexShaderSrc)){
        printf("failed to load vertex shader file\n");
        return 1;
    }

    if(load_text_file(fragmentPath, &fragmentShaderSrc)){
        printf("failed to load fragment shader file\n");
        return 1;
    }
    
    if(create_shader_program(program, vertexShaderSrc, fragmentShaderSrc)){
        printf("failed to create shader program\n");
        return 1;
    }
    return 0;
}
