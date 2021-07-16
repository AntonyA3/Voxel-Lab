
#include "../include/nuklear_gui.h"

void do_show_cursor_debug(struct nk_context *ctx, App app){
    if(nk_tree_push(ctx, NK_TREE_TAB, "Cursor", NK_MINIMIZED)){
        char str[40];
        nk_layout_row_static(ctx, 32, 64, 3);     
        nk_label(ctx, "clip", NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.clipPosition.x);
        nk_label(ctx, str, NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.clipPosition.y);
        nk_label(ctx, str, NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 32, 64, 4);     
        nk_label(ctx, "ray_o", NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.ray.x);
        nk_label(ctx, str, NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.ray.y);
        nk_label(ctx, str, NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.ray.z);
        nk_label(ctx, str, NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 32, 64, 4);     

        nk_label(ctx, "ray_d", NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.ray.dx);
        nk_label(ctx, str, NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.ray.dy);
        nk_label(ctx, str, NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.ray.dz);
        nk_label(ctx, str, NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 32, 64, 4);     
        nk_label(ctx, "hit_p", NK_TEXT_LEFT);

        sprintf(str, "%f", app.cursor.hitPosition.x);
        nk_label(ctx, str, NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.hitPosition.y);
        nk_label(ctx, str, NK_TEXT_LEFT);
        sprintf(str, "%f", app.cursor.hitPosition.z);
        nk_label(ctx, str, NK_TEXT_LEFT);

        nk_tree_pop(ctx);


    }
}

void do_show_orbit_camera_debug(struct nk_context *ctx, App app){
    if(nk_tree_push(ctx, NK_TREE_TAB, "Orbit Camera", NK_MINIMIZED)){
        char str[20];
     
        nk_layout_row_static(ctx, 32, 64, 2);
        
        nk_label(ctx, "in Vox", NK_TEXT_LEFT);
        sprintf(str, "%d", app.cameraInsideVoxel);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 32, 64, 3);

        nk_label(ctx, "yaw/pitch", NK_TEXT_LEFT);
        sprintf(str, "%f", app.orbitCamera.yaw);
        nk_label(ctx, str , NK_TEXT_LEFT);

        sprintf(str, "%f", app.orbitCamera.pitch);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 32, 94, 4);

        nk_label(ctx, "position", NK_TEXT_LEFT);
        sprintf(str, "%f", app.orbitCameraPosition.x);
        nk_label(ctx, str , NK_TEXT_LEFT);

        sprintf(str, "%f", app.orbitCameraPosition.y);
        nk_label(ctx, str , NK_TEXT_LEFT);

        sprintf(str, "%f", app.orbitCameraPosition.z);
        nk_label(ctx, str , NK_TEXT_LEFT);

        nk_tree_pop(ctx);
    }

}

void do_show_floor_grid_debug(struct nk_context *ctx, App app){
  if(nk_tree_push(ctx, NK_TREE_TAB, "Floor Grid", NK_MINIMIZED)){
        char str1[20], str2[20], str3[20];
        nk_layout_row_static(ctx, 32, 64, 4);
        
        nk_label(ctx, "iVec", NK_TEXT_LEFT);
        sprintf(str1, "%f", app.floorGrid.iVec[0]);
        sprintf(str2, "%f", app.floorGrid.iVec[1]);
        sprintf(str3, "%f", app.floorGrid.iVec[2]);
        nk_label(ctx, str1 , NK_TEXT_LEFT);
        nk_label(ctx, str2 , NK_TEXT_LEFT);
        nk_label(ctx, str3 , NK_TEXT_LEFT);

        nk_label(ctx, "jVec", NK_TEXT_LEFT);
        sprintf(str1, "%f", app.floorGrid.jVec[0]);
        sprintf(str2, "%f", app.floorGrid.jVec[1]);
        sprintf(str3, "%f", app.floorGrid.jVec[2]);
        nk_label(ctx, str1 , NK_TEXT_LEFT);
        nk_label(ctx, str2 , NK_TEXT_LEFT);
        nk_label(ctx, str3 , NK_TEXT_LEFT);

        nk_tree_pop(ctx);
    }
}

void do_show_voxel_debug(struct nk_context *ctx, App app){
    if(nk_tree_push(ctx, NK_TREE_TAB, "Voxel Store", NK_MINIMIZED)){
        char str[20];
        nk_layout_row_static(ctx, 32, 64, 2);
        nk_label(ctx, "Vox Count", NK_TEXT_LEFT);
        nk_label(ctx, str, NK_TEXT_LEFT);

        nk_tree_pop(ctx);
    }
}
void do_show_debug_window(struct nk_context *ctx, App app){
    if(nk_begin(ctx, "Debug Window ", nk_rect(0,0, 128,128),
        NK_WINDOW_BORDER |  NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |NK_WINDOW_TITLE |NK_WINDOW_MINIMIZABLE)){
            do_show_cursor_debug(ctx, app);
            do_show_orbit_camera_debug(ctx, app);
            do_show_floor_grid_debug(ctx, app);
            do_show_voxel_debug(ctx, app);
    }
    nk_end(ctx);
}

void do_show_file_menu(struct nk_context *ctx, App *app){
    struct nk_vec2 size;
    size.x = app->fileMenuSize[0];
    size.y = app->fileMenuSize[1];
    if(nk_menu_begin_label(ctx, "File",NK_TEXT_CENTERED, size)){
        nk_layout_row_dynamic(ctx, 25,1);
        nk_menu_item_label(ctx, "New", NK_TEXT_LEFT);
        nk_menu_item_label(ctx, "Open", NK_TEXT_LEFT);
        nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT);
        if(nk_menu_item_label(ctx, "Quit", NK_TEXT_LEFT)){
            
        }
        nk_menu_end(ctx);
    }
}

void do_show_menu_bar(struct nk_context *ctx, App *app){
    nk_layout_row_static(ctx, app->menuBarHeight, app->menuBarButtonWidth, MENU_BAR_ITEM_COUNT);
    do_show_file_menu(ctx, app);

}

void do_show_property_selector_panel(struct nk_context *ctx, App *app){
    nk_layout_row_push(ctx, app->propertySelectorWidth);
    if(nk_group_begin(ctx, "Property Selector",NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){  
        

        nk_group_end(ctx);
    }

}

void do_show_properties_panel(struct nk_context *ctx, App *app){
    nk_layout_row_push(ctx, app->propertyWidth);
    if(nk_group_begin(ctx, "Properties ",NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
        nk_layout_row_static(ctx, 32, 64, 2);
        int addVoxSelected = (app->voxelManipulationMode == VOXEL_EDIT_ADD_VOXEL) ? 
            nk_true: nk_false;  
        if(nk_select_label(ctx, "addVox",NK_TEXT_ALIGN_LEFT, addVoxSelected)){
            app->voxelManipulationMode = VOXEL_EDIT_ADD_VOXEL;
        }
        int deleteVoxSelected = (app->voxelManipulationMode == VOXEL_EDIT_DELETE_VOXEL) ? 
            nk_true: nk_false;

        if(nk_select_label(ctx, "deleteVox",NK_TEXT_ALIGN_LEFT, deleteVoxSelected)){
            app->voxelManipulationMode = VOXEL_EDIT_DELETE_VOXEL;
        }
        nk_group_end(ctx);
    }
}
void do_show_render_panel(struct nk_context *ctx, App *app){
    nk_layout_row_push(ctx, app->renderPanelWidth);
    
    if(nk_group_begin(ctx, "Render View ",NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)){
        nk_layout_row_static(ctx,app->renderViewport.height, app->renderViewport.width, 1);
        struct nk_image image = nk_image_id(app->frameTexture);
        struct nk_color imageColor = {0xFF,0xFF,0xFF,0xFF};
        nk_image_color(ctx,image, imageColor);
        nk_group_end(ctx);
    }


}

void do_show_main_window(struct nk_context *ctx, App *app)
{
    
    nk_style_push_float(ctx, &ctx->style.window.padding.x, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.padding.y, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.spacing.x, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.spacing.y, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.group_padding.x, 0.0f);
    nk_style_push_float(ctx, &ctx->style.window.group_padding.y, 0.0f);

    if(nk_begin(ctx, "Main Window", nk_rect(0,0,app->windowViewport.width, app->windowViewport.height),
    NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND |NK_WINDOW_BORDER) ){
        do_show_menu_bar(ctx, app);
        nk_layout_row_begin(ctx, NK_STATIC, app->windowViewport.height - app->menuBarHeight,3);
        do_show_property_selector_panel(ctx, app);
        do_show_properties_panel(ctx, app);
        do_show_render_panel(ctx, app);
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
};



