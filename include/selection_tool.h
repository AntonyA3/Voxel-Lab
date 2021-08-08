#ifndef SELECTION_TOOL_H
#define SELECTION_TOOL_H
#include "../include/aabb.h"
#include "../include/move_tool.h"
#include "../include/box_scale_tool.h"
#include "../include/selection_tool_state.h"
#include "../include/button_state.h"
#include "../include/tool_id.h"
typedef struct
{

    int subTool;
    int active;
    int state;
    int shape;
    int selectedEntity;
    VoxelTree *selectedVoxels;
    Aabb selectionBox;

}SelectionTool;

void selection_tool_update(SelectionTool *selectionTool, MoveTool *moveTool,
    BoxScaleTool *boxScaleTool, int buttonState, vec3 lookVector, Ray picker);
#endif