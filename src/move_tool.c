#include "../include/move_tool.h"


Plane move_tool_get_hit_plane(MoveTool moveTool){
    Plane plane;
    switch (moveTool.axis)
    {
    case MOVE_TOOL_AXIS_X:
        aabb_get_centre(moveTool.xPulley, plane.point);
        plane.normal[0] = 0;
        plane.normal[1] = 1;
        plane.normal[2] = 0;
        break;
    case MOVE_TOOL_AXIS_Y:
        aabb_get_centre(moveTool.yPulley, plane.point);
        plane.normal[0] = 0;
        plane.normal[1] = 0;
        plane.normal[2] = 1;
        break;
    case MOVE_TOOL_AXIS_Z:
        aabb_get_centre(moveTool.zPulley, plane.point);
        plane.normal[0] = 0;
        plane.normal[1] = 1;
        plane.normal[2] = 0;
        break;
    }
    return plane;
}

void move_tool_move(MoveTool *moveTool, float *x, float *y, float *z){
    switch (moveTool->axis){
    case MOVE_TOOL_AXIS_X:
        {
        float dx = moveTool->x - moveTool->prevX;
        *x += dx;
        moveTool->origin[0] += dx;
        }
        break;
    case MOVE_TOOL_AXIS_Y:
        {
        float dy = moveTool->y - moveTool->prevY;
        *y += dy;
        moveTool->origin[1] += dy;
        }
        break;
    case MOVE_TOOL_AXIS_Z:
        {
        float dz = moveTool->z - moveTool->prevZ;
        *z += dz;
        moveTool->origin[2] += dz;
        }
        break;
    }  
}
void move_tool_select_axis(MoveTool *moveTool, Ray picker){
    int side;
    float distance = 1000;
    if(ray_vs_aabb(picker, moveTool->xPulley, &distance, &side)){            
        moveTool->axis = MOVE_TOOL_AXIS_X;
    }
    if(ray_vs_aabb(picker, moveTool->yPulley, &distance, &side)){
        moveTool->axis = MOVE_TOOL_AXIS_Y;
    }
    if(ray_vs_aabb(picker, moveTool->zPulley, &distance, &side)){
        moveTool->axis = MOVE_TOOL_AXIS_Z;
    }           
}

void move_tool_move_point(MoveTool *moveTool, Ray picker, float *x, float*y, float *z, int buttonState){
    switch (moveTool->state)
    {
    case MOVE_TOOL_STATE_NOT_MOVING:   
        if(buttonState == BUTTON_STATE_PRESSED){
            move_tool_select_axis(moveTool, picker);
            Plane hitPlane = move_tool_get_hit_plane(*moveTool);
            vec3 hitpoint;
            float distance = 1000;
            int side;
            if(ray_vs_plane(picker, hitPlane, &distance, &side)){
                ray_get_point_at_distance(picker, distance, hitpoint);
                memcpy(moveTool->pos, hitpoint, sizeof(vec3)); 
                memcpy(moveTool->prev, hitpoint, sizeof(vec3));
                moveTool->state = MOVE_TOOL_STATE_MOVING; 
            }         
        }
        break;
    case MOVE_TOOL_STATE_MOVING:
        if(buttonState == BUTTON_STATE_DOWN){
            Plane hitPlane = move_tool_get_hit_plane(*moveTool);
            int side;
            float distance = 1000;        

            if(ray_vs_plane(picker, hitPlane, &distance, &side)){
                ray_get_point_at_distance(picker, distance, moveTool->pos);
                move_tool_move(moveTool, x, y, z);
                memcpy(&moveTool->prev, &moveTool->pos, sizeof(vec3));
            }
        }
        if(buttonState == BUTTON_STATE_UP){
            moveTool->state = MOVE_TOOL_STATE_NOT_MOVING;
        }
        break;
    }
}

void move_tool_move_aabb(MoveTool *moveTool, Ray picker, Aabb *aabb, int buttonState){
    vec3 center, halfExtent;
    aabb_get_centre(*aabb, center);
    move_tool_move_point(moveTool, picker, &center[0], &center[1], &center[2], 
        buttonState);
    vec3_scale(halfExtent,aabb->extents, 0.5);
    *aabb = aabb_from_origin_halfExtents(center, halfExtent);
}

void move_tool_move_voxel_tree(MoveTool *moveTool, Ray picker, VoxelTree *voxelTree, int buttonState){
    Aabb voxBox = voxel_to_aabb(*voxelTree->head);
    Aabb newBox;
    memcpy(&newBox, &voxBox, sizeof(Aabb));
    move_tool_move_aabb(moveTool, picker, &newBox, buttonState);
    
    voxelTree->offset[0] += newBox.x - voxBox.x;
    voxelTree->offset[1] += newBox.y - voxBox.y;
    voxelTree->offset[2] += newBox.z - voxBox.z;


}