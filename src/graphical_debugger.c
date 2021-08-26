#include "../include/graphical_debugger.h"

void graphical_debugger_push_aabb(GraphicalDebugger *graphicalDebugger,Aabb aabb, ColorRGBAF color){
    DebugAabb *aabbDebug = (DebugAabb*)malloc(sizeof(DebugAabb));
    aabbDebug->itemType = DEBUG_AABB;
    aabbDebug->aabb = aabb;
    memcpy(&aabbDebug->color, &color, sizeof(ColorRGBAF));

    if(graphicalDebugger->debugItems == NULL){
        graphicalDebugger->debugItems = aabbDebug;
    }else{
        aabbDebug->nextItem = graphicalDebugger->debugItems;
        graphicalDebugger->debugItems = aabbDebug;
    }
}

void graphic_debugger_push_voxel(GraphicalDebugger *graphicalDebugger, Voxel *voxel, ColorRGBAF color){
    switch (voxel->size)
    {
    case 1:
        {
            ColorRGBAF color = {1.0,0.0,1.0,1.0};
            graphical_debugger_push_aabb(graphicalDebugger, voxel_to_aabb(voxel), color);
        }
        break;
    default:
        {
            
            if(voxel_is_leaf(voxel)){
                ColorRGBAF color = {1.0,0.0,1.0,1.0};
                graphical_debugger_push_aabb(graphicalDebugger, voxel_to_aabb(voxel), color);
            }else{
                ColorRGBAF color = {1.0,0.5,0.0,1.0};
                graphical_debugger_push_aabb(graphicalDebugger, voxel_to_aabb(voxel), color);
                for(int i = 0; i < 8; i++){
                    if(voxel->child[i] != NULL){
                        graphic_debugger_push_voxel(graphicalDebugger, voxel->child[i], color);
                    }
                }
            }
        }
        break;
    }
}


void graphical_debugger_push_sparce_voxel(GraphicalDebugger *graphicalDebugger, SparceVoxel *voxel, ColorRGBAF color){
    switch (voxel->type)
    {
    case SPARCE_VOXEL_LARGE:
        {
            SparceVoxelLarge* vox = (SparceVoxelLarge*)voxel;
            for(int i = 0; i < 8; i++){
                if(vox->child[i] != NULL){
                    {
                        ColorRGBAF color = {1.0,1.0,0.0,1.0};
                        Aabb aabb = voxel_large_get_aabb(vox);
                        //graphical_debugger_push_aabb(graphicalDebugger, aabb, color);
                    }
                    graphical_debugger_push_sparce_voxel(graphicalDebugger, vox->child[i], color);
                }
            }
        }
        break;
    case SPARCE_VOXEL_16X16X16:
        {
            SparceVoxel16x16x16* vox = (SparceVoxel16x16x16*)voxel;
            {
                ColorRGBAF color = {0.0,0.25,0.5,1.0};
                Aabb aabb = {vox->min[0], vox->min[1], vox->min[2], 16,16,16};
                //graphical_debugger_push_aabb(graphicalDebugger, aabb, color);
            }
            
            ColorRGBA colorArray[16][16][16];
            ColorRGBA *colorArrayFlat = &colorArray[0][0][0];
            {
                int count = 0;
                ColorEncodingNode **currentColor = &((ColorRLE*)(vox)->colorData)->head;
                count = (*currentColor)->encoding.colorCount;

                for(int i = 0; i < 16 * 16 * 16; i++){
                    if((*currentColor) != NULL){
                        colorArrayFlat[i].colorI = (*currentColor)->encoding.color.colorI;
                        count -= 1;

                        if(count == 0){

                            if((*currentColor)->nextColor != NULL){
                                currentColor = &(*currentColor)->nextColor;
                                count = (*currentColor)->encoding.colorCount;
                            }

                        }
                    }
                    
                }
            }

            for(int x = 0; x < 16; x++){
                for(int y = 0; y < 16; y++){
                    for(int z = 0; z < 16; z++){
                        if(colorArray[x][y][z].a != 0){
                            ColorRGBAF col = colorRgba_to_colorRgbaf(colorArray[x][y][z]);
                            Aabb aabb = {x + vox->min[0], y + vox->min[1], z + vox->min[2], 1, 1, 1};
                            graphical_debugger_push_aabb(graphicalDebugger, aabb, col);
                        }
                    }
                }   
            }   
        }
        break;
    }
}

void graphical_debugger_items_clear(DebugItem **item){
    if((*item)->nextItem == NULL){
        free(*item);
        *item = NULL;
    }else{
        graphical_debugger_items_clear(&(*item)->nextItem);
        free(*item);
        *item = NULL;
    }
}
void graphical_debugger_clear(GraphicalDebugger *graphicalDebugger){

    if(graphicalDebugger->debugItems != NULL){
       graphical_debugger_items_clear(&graphicalDebugger->debugItems);
    }
}

void graphical_debuggerItemCount(void *item, int *vertexCount, int *elementCount){
    DebugItem* debugItem = (DebugItem*)item;
    switch (debugItem->itemType)
    {
    case DEBUG_AABB:
        *vertexCount += 24;
        *elementCount += 36;
        break;
    }

    if(debugItem->nextItem){
        graphical_debuggerItemCount(debugItem->nextItem, vertexCount, elementCount);
    }
}

void graphical_debugger_get_item_draw_data(void *item, float *verticies, unsigned int *elements, int *elementStride){
    DebugItem *debugItem = (DebugItem*)item;
    switch (debugItem->itemType)
    {
        case DEBUG_AABB:
        {
            DebugAabb *aabbItem = (DebugAabb*)item;
            model_generate_aabb_vertex_array(verticies, aabbItem->aabb, aabbItem->color.color, VERTEX_POSITION + VERTEX_RGBA);
            model_generate_aabb_element_array(elements,0);
            
            for(int i = 0; i < 36; i++){
                elements[i] += *elementStride;
            }
            if(debugItem->nextItem){
                *elementStride += 24;
                graphical_debugger_get_item_draw_data(debugItem->nextItem, &verticies[24 * 7], &elements[36], elementStride);
            }
        }
        break;
    }
}

void graphical_debugger_get_size(GraphicalDebugger *graphicalDebugger, int *floatCount, int *elementCount){
    
    int vertexCount = 0;
    *elementCount = 0;
    graphical_debuggerItemCount(graphicalDebugger->debugItems, &vertexCount, elementCount);
    *floatCount = vertexCount * 7;
}

void graphical_debugger_draw(GraphicalDebugger* graphicalDebugger){
    
    if(graphicalDebugger->debugItems != NULL){

        int floatCount, elementCount;
        graphical_debugger_get_size(graphicalDebugger, &floatCount, &elementCount);

        float *verticies = (float*) malloc(sizeof(float) * floatCount);
        unsigned int *elements = (unsigned int*) malloc(sizeof(unsigned int) * elementCount);
        
        int elementStride = 0;
        graphical_debugger_get_item_draw_data(graphicalDebugger->debugItems, verticies, elements, &elementStride);
        
        glBindBuffer(GL_ARRAY_BUFFER, graphicalDebugger->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(float), verticies, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphicalDebugger->elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount * sizeof(unsigned int), elements, GL_DYNAMIC_DRAW);
        
        glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);

        free(verticies);
        free(elements);    
    }
}