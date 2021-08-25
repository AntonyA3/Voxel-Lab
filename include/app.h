#ifndef APP_H
#define APP_H
#include <stdio.h>
#include <stdlib.h>
#include "../include/frame_buffer_object.h"
//#include "../include/app_event_stack.h"
#include "../include/world.h"

#define MODEL_COUNT 5
#define SHADER_COUNT 2



enum property{PROPERTY_TOOL, PROPERTY_CAMERA, PROPERTY_SCENE};
//enum property_catagory{EDITOR_PROPERTY, TOOL_PROPERTY};
enum panel_focus{PANEL_FOCUS_APP, PANEL_FOCUS_GUI};

typedef struct
{
    FrameBufferObject appViewFrame;
    //AppEventStack appEventStack;
    World world;
    int panelFocused;
    int isCameraRotate;
    int toolSelected;
    int propertySelected;
    int propertyCatagory;
    union
    {
        struct 
        {
            GLuint flatShader, phongShader;
        };
        GLuint *shaders[SHADER_COUNT]
    };
}App;

void file_get_text(const char *path, char **text);
GLuint shader_generate(const char *vertexShaderPath, const char *fragmentShaderPath);
#endif
