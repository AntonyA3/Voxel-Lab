#ifndef BOX_SCALE_TOOL_H
#define BOX_SCALE_TOOL_H
#include "../linmath/linmath.h"
#include "../include/aabb.h"
#include "../include/button_state.h"
#include "../include/ray.h"
#include "../include/box_scale_tool_state.h"
#include "../include/box_scale_tool_side.h"
typedef struct{
    int side;
    int state;
    vec3 prev;
    vec3 pos;
    Aabb leftPulley;
    Aabb rightPulley;
    Aabb bottomPulley;
    Aabb topPulley;
    Aabb frontPulley;
    Aabb backPulley;

}BoxScaleTool;

void box_scale_tool_scale(BoxScaleTool *boxScaleTool, Ray picker, Aabb *aabb);
void box_scale_tool_pick_side(BoxScaleTool *boxScaleTool, Ray picker);
void box_scale_tool_aabb(BoxScaleTool *boxScaleTool, Ray picker, Aabb *aabb, int buttonState);

/*
box_scale_tool_aabb(&app->boxScaleTool, app->cursorRay, &app->selectionTool.selectionBox, 
        app->cursorButtonState[CURSOR_BUTTON_ID_PRIMARY]);
*/
Plane box_scale_get_plane(BoxScaleTool *boxScaleTool);
#endif