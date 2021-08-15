#ifndef APP_H
#define APP_H
#include "../include/gui_window.h"
#include "../include/frame.h"
#include "../include/grid.h"
#include "../include/model.h"
#include "../include/camera.h"
#include "../include/cursor.h"
#include "../include/voxel_tree.h"
#include "../include/voxel_editor.h"
#include "../include/scene.h"
#define MODEL_COUNT 5

enum property{PROPERTY_CAMERA, PROPERTY_SCENE};
enum tool{TOOL_PAINT, TOOL_ERASE};
typedef struct
{
    GuiWindow guiWindow;
    Frame frame;
    Grid floorGrid;
    VoxelTree mainVoxels;
    Scene scene;
    union 
    {   struct{
            Model floorGridModel, testTriangle, hitModel, voxelBox, voxelModel;
        };
        Model models[MODEL_COUNT];
    };
    Camera camera;
    Cursor cursor;

    VoxelEditor voxelEditor;
    int isCameraRotate;
    int toolSelected;
    int propertySelected;
    
    GLuint flatShader, phongShader;
}App;
#endif
