#include "../include/app.h"


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

