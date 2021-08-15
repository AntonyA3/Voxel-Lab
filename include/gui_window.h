#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H
#include "../include/rect.h"
#include "../include/prop_tab_item.h"
typedef struct 
{
    float width;
    float height;
    struct
    {
        float height;
    }menuBar;
    struct 
    {
        float height;
        struct 
        {
            PropTabItem *propTabItem;
            float width;
        }propTab;
        struct 
        {
            float width;
        }selectorTab;

        
        struct
        {
            float x, y, width, height;
            float ratio;
        }appView;
        
        
    }windowContent;
    
      
}GuiWindow;

void gui_window_update(GuiWindow *guiWindow, float width, float height){
    guiWindow->width = width;
    guiWindow->height = height;
    guiWindow->menuBar.height = 32;
}
#endif