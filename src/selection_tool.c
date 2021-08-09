#include "../include/selection_tool.h"

/*
void selection_tool_update(SelectionTool *selectionTool, MoveTool *moveTool,
    BoxScaleTool *boxScaleTool, int buttonState, vec3 lookVector, Ray picker){   
    switch (selectionTool->state)
    {
    case SELECTION_TOOL_STATE_PICK_ORIGIN:
        {
            vec3 origin;
            vec3 halfExtents = {2.5,2.5,2.5};
            selectionTool->state = SELECTION_TOOL_STATE_PICK_AREA;
            vec3_scale(origin, lookVector, 10);
            vec3_add(origin, origin, picker.origin);
            selectionTool->selectionBox = aabb_from_origin_halfExtents(origin, halfExtents);
            aabb_get_centre(selectionTool->selectionBox, moveTool->origin);
        }
        break;    
    case SELECTION_TOOL_STATE_PICK_AREA:
        switch (selectionTool->subTool)
        {
        case TOOL_ID_MOVE_TOOL:
            move_tool_move_aabb(moveTool, picker, &selectionTool->selectionBox, buttonState);
            break;
        case TOOL_ID_BOX_SCALE_TOOL:
            box_scale_tool_aabb(boxScaleTool, picker, &selectionTool->selectionBox, buttonState);
            break;
        }
        break;
    case SELECTION_TOOL_STATE_VOXEL_SELECTED:
        switch (selectionTool->subTool)
        {
        case TOOL_ID_MOVE_TOOL:
            {
                Aabb startBox;
                memcpy(&startBox, &selectionTool->selectionBox.min, sizeof(Aabb));
                move_tool_move_aabb(moveTool, picker, &selectionTool->selectionBox, buttonState);
                selectionTool->selectedVoxels->offset[0] += selectionTool->selectionBox.x - startBox.x;
                selectionTool->selectedVoxels->offset[1] += selectionTool->selectionBox.y - startBox.y;
                selectionTool->selectedVoxels->offset[2] += selectionTool->selectionBox.z - startBox.z;

            }
            break;
        case TOOL_ID_BOX_SCALE_TOOL:
            break;
        }
    }
}
*/