#ifndef GRAPHICAL_DEBUGGER_H
#define GRAPHICAL_DEBUGGER_H
#include <GL/glew.h>
#include "../include/shape.h"
#include "../include/model.h"
#include "../include/color.h"
#include "../include/sparce_voxels/sparce_voxel_octree.h"
#include "../include/voxel_octree.h"
#include "../include/stack.h"
enum graphical_debug_item{DEBUG_AABB};


typedef struct DebugItem{
    int itemType;
    struct DebugItem *nextItem;
}DebugItem;

typedef struct DebugAabb{
    int itemType;
    DebugItem *nextItem;
    Aabb aabb;
    ColorRGBAF color;
}DebugAabb;



typedef struct GraphicalDebugger
{
    DebugItem *debugItems;
    GLuint vertexBuffer;
    GLuint elementBuffer;
}GraphicalDebugger;

void graphic_debugger_push_voxel(GraphicalDebugger *graphicalDebugger, Voxel *voxel, ColorRGBAF color);
void graphical_debugger_push_aabb(GraphicalDebugger *graphicalDebugger,Aabb aabb, ColorRGBAF color);
void graphical_debugger_push_sparce_voxel(GraphicalDebugger *graphicalDebugger, SparceVoxel *voxel, ColorRGBAF color);

void graphical_debugger_items_clear(DebugItem **item);
void graphical_debugger_clear(GraphicalDebugger *graphicalDebugger);
void graphical_debugger_get_size(GraphicalDebugger *graphicalDebugger, int *floatCount, int *elementCount);
void graphical_debugger_draw(GraphicalDebugger* graphicalDebugger);
#endif

