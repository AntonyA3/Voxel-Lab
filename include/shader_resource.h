
#include <stdio.h>
#include <GL/glew.h>

int create_shader_program(GLuint *program, const char *vertShadeSrc, const char *fragShadeSrc);
int load_text_file(const char* path, const char** response);
int load_shader_program(GLuint* program,const char* vertexPath,const char* fragmentPath);