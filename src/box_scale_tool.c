#include "../include/box_scale_tool.h"

void box_scale_tool_pick_side(BoxScaleTool *boxScaleTool,Ray picker){
    float distance = 1000;
    int side;
    
    if(ray_vs_aabb(picker, boxScaleTool->leftPulley, &distance, &side)){
        boxScaleTool->side =  BOX_SCALE_TOOL_SIDE_LEFT;
    }

    if(ray_vs_aabb(picker, boxScaleTool->rightPulley, &distance, &side)){
        boxScaleTool->side =  BOX_SCALE_TOOL_SIDE_RIGHT;
    }

    if(ray_vs_aabb(picker, boxScaleTool->bottomPulley, &distance, &side)){
        boxScaleTool->side =  BOX_SCALE_TOOL_SIDE_BOTTOM;
    }

    if(ray_vs_aabb(picker, boxScaleTool->topPulley, &distance, &side)){
        boxScaleTool->side =  BOX_SCALE_TOOL_SIDE_TOP;
    }

    if(ray_vs_aabb(picker, boxScaleTool->frontPulley, &distance, &side)){
        boxScaleTool->side =  BOX_SCALE_TOOL_SIDE_FRONT;
    }

    if(ray_vs_aabb(picker, boxScaleTool->backPulley, &distance, &side)){
        boxScaleTool->side =  BOX_SCALE_TOOL_SIDE_BACK;
    }
    
}

void box_scale_tool_scale(BoxScaleTool *boxScaleTool, Ray picker, Aabb *aabb){
    vec3 hitpoint;
    float distance = 1000;
    int side;
    Plane plane = box_scale_get_plane(boxScaleTool);
    
    if(ray_vs_plane(picker, plane, &distance, &side)){
        ray_get_point_at_distance(picker, distance, hitpoint);
        switch (boxScaleTool->side)
        {
        case BOX_SCALE_TOOL_SIDE_LEFT:
            {
            memcpy(boxScaleTool->pos, hitpoint, sizeof(vec3));
            float dx = boxScaleTool->pos[0] - boxScaleTool->prev[0];
            dx = minf(dx, aabb->w - 0.1);
            aabb->w -= dx;
            aabb->x += dx;
            memcpy(boxScaleTool->prev, boxScaleTool->pos, sizeof(vec3));
            }
            break;
        case BOX_SCALE_TOOL_SIDE_RIGHT:
            {
            memcpy(boxScaleTool->pos, hitpoint, sizeof(vec3));
            float dx = boxScaleTool->pos[0] - boxScaleTool->prev[0];
            aabb->w = maxf(aabb->w + dx, 0.1);
            memcpy(boxScaleTool->prev, boxScaleTool->pos, sizeof(vec3));
            }
            break;  
        case BOX_SCALE_TOOL_SIDE_BOTTOM:
            {
            memcpy(boxScaleTool->pos, hitpoint, sizeof(vec3));
            float dy = boxScaleTool->pos[1] - boxScaleTool->prev[1];
            dy = minf(dy, aabb->h - 0.1);
            aabb->h -= dy;
            aabb->y += dy;
            memcpy(boxScaleTool->prev, boxScaleTool->pos, sizeof(vec3));
            }
            break;
        case BOX_SCALE_TOOL_SIDE_TOP:
            {
            memcpy(boxScaleTool->pos, hitpoint, sizeof(vec3));
            float dy = boxScaleTool->pos[1] - boxScaleTool->prev[1];
            aabb->h = maxf(aabb->h + dy, 0.1);
            memcpy(boxScaleTool->prev, boxScaleTool->pos, sizeof(vec3));
            }
            break;
        case BOX_SCALE_TOOL_SIDE_FRONT:
            {
            memcpy(boxScaleTool->pos, hitpoint, sizeof(vec3));
            float dz = boxScaleTool->pos[2] - boxScaleTool->prev[2];
            dz = minf(dz, aabb->d - 0.1);
            aabb->d -= dz;
            aabb->z += dz;
            memcpy(boxScaleTool->prev, boxScaleTool->pos, sizeof(vec3));
            }
            break;
        case BOX_SCALE_TOOL_SIDE_BACK:
            {
            memcpy(boxScaleTool->pos, hitpoint, sizeof(vec3));
            float dz = boxScaleTool->pos[2] - boxScaleTool->prev[2];
            aabb->d = maxf(aabb->d + dz, 0.1);
            memcpy(boxScaleTool->prev, boxScaleTool->pos, sizeof(vec3));
            }
            break;
        }
    }
}

void box_scale_tool_aabb(BoxScaleTool *boxScaleTool, Ray picker, Aabb *aabb, int buttonState){
    switch (boxScaleTool->state)
    {
    case BOX_SCALE_TOOL_NOT_SCALING:
        if(buttonState == BUTTON_STATE_PRESSED){
            float distance = 1000;
            int side;
            vec3 hitpoint;
            box_scale_tool_pick_side(boxScaleTool, picker);
            Plane plane = box_scale_get_plane(boxScaleTool);
            if(ray_vs_plane(picker,plane, &distance, &side)){
                ray_get_point_at_distance(picker, distance, hitpoint);
                memcpy(boxScaleTool->prev,hitpoint, sizeof(vec3));
                memcpy(boxScaleTool->pos,hitpoint, sizeof(vec3));
                boxScaleTool->state = BOX_SCALE_TOOL_SCALING;
            }
        }
        break;
    case BOX_SCALE_TOOL_SCALING:  
        if(buttonState == BUTTON_STATE_DOWN){
            box_scale_tool_scale(boxScaleTool, picker, aabb);
        }
        if(buttonState == BUTTON_STATE_UP){
            boxScaleTool->state = BOX_SCALE_TOOL_NOT_SCALING;
        }
        break;
    }
}

Plane box_scale_get_plane(BoxScaleTool *boxScaleTool){
    Plane plane;
    switch (boxScaleTool->side)
    {
    case BOX_SCALE_TOOL_SIDE_LEFT:  
        aabb_get_centre(boxScaleTool->leftPulley, plane.point);
        plane.normal[0] = 0;
        plane.normal[1] = 1;
        plane.normal[2] = 0;
        break;              
    case BOX_SCALE_TOOL_SIDE_RIGHT:
        aabb_get_centre(boxScaleTool->rightPulley, plane.point);
        plane.normal[0] = 0;
        plane.normal[1] = 1;
        plane.normal[2] = 0;
        break;
    case BOX_SCALE_TOOL_SIDE_BOTTOM:
        aabb_get_centre(boxScaleTool->bottomPulley, plane.point);
        plane.normal[0] = 1;
        plane.normal[1] = 0;
        plane.normal[2] = 0;
        break;
    case BOX_SCALE_TOOL_SIDE_TOP:
        aabb_get_centre(boxScaleTool->topPulley, plane.point);
        plane.normal[0] = 1;
        plane.normal[1] = 0;
        plane.normal[2] = 0;
        break;
    case BOX_SCALE_TOOL_SIDE_FRONT:
        aabb_get_centre(boxScaleTool->frontPulley, plane.point);
        plane.normal[0] = 0;
        plane.normal[1] = 1;
        plane.normal[2] = 0;
        break;
    case BOX_SCALE_TOOL_SIDE_BACK:
        aabb_get_centre(boxScaleTool->backPulley, plane.point);
        plane.normal[0] = 0;
        plane.normal[1] = 1;
        plane.normal[2] = 0;
        break;
    }
    return plane;
}
