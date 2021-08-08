#ifndef MOVE_TOOL_H
#define MOVE_TOOL_H
#include "../linmath/linmath.h"
#include "../include/aabb.h"
#include "../include/move_tool_axis.h"
#include "../include/move_tool_state.h"
#include "../include/plane.h"
#include "../include/ray.h"
#include "../include/button_state.h"

typedef struct 
{
    int state;
    int axis;
    union{
        struct
        {
            float prevX, prevY, prevZ, x, y, z;
        };
        struct
        {
            vec3 prev;
            vec3 pos;
        };
    };    
    vec3 origin;
    Aabb xPulley;
    Aabb yPulley;
    Aabb zPulley;

}MoveTool;

void move_tool_move_point(MoveTool *moveTool, Ray picker, float *x, float*y, float *z, int buttonState);
void move_tool_move_aabb(MoveTool *MoveTool, Ray picker, Aabb *aabb, int buttonState);
void move_tool_move_voxel_tree(MoveTool *MoveTool, Ray picker, VoxelTree *voxelTree, int buttonState);
void move_tool_update_y_pulley();


#endif