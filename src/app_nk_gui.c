#include "../include/app_nk_gui.h"

void app_nk_gui_menu_bar(App *app, struct nk_context *ctx){
    nk_menubar_begin(ctx);            
    struct nk_vec2 s = {64, 64};
    nk_layout_row_static(ctx, 32, app->menuBarHeight, 3);
    if(nk_menu_begin_label(ctx, "File", NK_TEXT_LEFT, s)){
        nk_layout_row_dynamic(ctx, 25,1);
        nk_menu_item_label(ctx, "New", NK_TEXT_LEFT);
        if(nk_menu_item_label(ctx, "Quit", NK_TEXT_LEFT)){
            app_quit(app);
        }
        nk_menu_end(ctx);
    }
    if(nk_menu_begin_label(ctx, "Edit", NK_TEXT_CENTERED, s)){
        nk_layout_row_dynamic(ctx, 25,1);
        nk_menu_item_label(ctx, "Undo", NK_TEXT_LEFT);
        nk_menu_item_label(ctx, "Redo", NK_TEXT_LEFT);

        nk_menu_end(ctx);
    }
    if(nk_menu_begin_label(ctx, "About", NK_TEXT_LEFT, s)){
        nk_menu_item_label(ctx, "Source Code", NK_TEXT_LEFT);
        nk_menu_end(ctx);
    }
    
    nk_menubar_end(ctx);
}

void app_nk_gui_voxel_shape_selector(App *app, struct nk_context *ctx){
    /*
    nk_layout_row_static(ctx, 32, 32, 3);
    
    nk_bool pred = (app->voxelEditor.shape == VOXEL_EDITOR_SHAPE_ID_POINT) ? nk_true : nk_false;
    if(nk_select_label(ctx,"point", NK_TEXT_LEFT, pred)){
        app->voxelEditor.shape = VOXEL_EDITOR_SHAPE_ID_POINT;
    }
    pred = (app->voxelEditor.shape == VOXEL_EDITOR_SHAPE_ID_CUBE) ? nk_true : nk_false;

    if(nk_select_label(ctx,"cube", NK_TEXT_LEFT, pred)){
        app->voxelEditor.shape = VOXEL_EDITOR_SHAPE_ID_CUBE;

    }
    pred = (app->voxelEditor.shape == VOXEL_EDITOR_SHAPE_ID_SPHERE) ? nk_true : nk_false;

    if(nk_select_label(ctx,"sphere", NK_TEXT_LEFT, pred)){
        app->voxelEditor.shape = VOXEL_EDITOR_SHAPE_ID_SPHERE;
    }
    */
}

void app_nk_gui_voxel_point_props(App *app, struct nk_context *ctx){

}

void app_nk_gui_voxel_cube_props(App *app, struct nk_context *ctx){
    /*
    float width = app->voxelEditor.aabb.w;
    float height = app->voxelEditor.aabb.h;
    float depth = app->voxelEditor.aabb.d;
    if(nk_group_begin(ctx, "Cube Properties", NK_WINDOW_BORDER)){

        nk_layout_row_static(ctx, 32, 128,1);
        nk_property_float(ctx, "#width",0,&width,255,0.01, 0.1);
        nk_property_float(ctx, "#height",0,&height,255,0.01, 0.1);
        nk_property_float(ctx, "#depth",0,&depth,255,0.01, 0.1);

        nk_group_end(ctx);   
    }
    app->voxelEditor.aabb.w = width;
    app->voxelEditor.aabb.h = height;
    app->voxelEditor.aabb.d = depth;
    */
}

void app_nk_gui_voxel_sphere_props(App *app, struct nk_context *ctx){
    /*
    float rad = app->voxelEditor.sphere.r;
    if(nk_group_begin(ctx, "Sphere Properties", NK_WINDOW_BORDER)){
        nk_layout_row_static(ctx, 32, 128,1);
        nk_property_float(ctx, "#radius",0,&rad,255,0.01, 0.1);
        nk_group_end(ctx);   
    }
    app->voxelEditor.sphere.r = rad;
    */

}

void app_nk_gui_voxel_editor_props(App *app, struct nk_context *ctx){
    /*{    
                
        nk_layout_row_static(ctx, 32, 32, 2);
        
        nk_bool pred = (app->voxelEditor.editMode == VOXEL_EDIT_MODE_ID_ADD) ? nk_true : nk_false;
        if(nk_select_label(ctx,"addVoxel", NK_TEXT_LEFT, pred)){
            app->voxelEditor.editMode = VOXEL_EDIT_MODE_ID_ADD;
        }
        
        pred = (app->voxelEditor.editMode == VOXEL_EDIT_MODE_ID_DELETE) ? nk_true : nk_false;
        if(nk_select_label(ctx,"deleteVoxel", NK_TEXT_LEFT, pred)){
            app->voxelEditor.editMode = VOXEL_EDIT_MODE_ID_DELETE;
        }
    }
    nk_layout_row_static(ctx, 32, 32, 1);

    nk_label(ctx, "dynamics", NK_TEXT_LEFT);
    {
        
        nk_layout_row_static(ctx, 32, 32, 3);
        
        nk_bool pred = (app->voxelEditor.dynamics == VOXEL_EDIT_DYNAMIC_PLOP) ? nk_true : nk_false;
        if(nk_select_label(ctx,"plop", NK_TEXT_LEFT, pred)){
            app->voxelEditor.dynamics = VOXEL_EDIT_DYNAMIC_PLOP;
        }
        
        pred = (app->voxelEditor.dynamics == VOXEL_EDIT_DYNAMIC_FLOW) ? nk_true : nk_false;
        if(nk_select_label(ctx,"flow", NK_TEXT_LEFT, pred)){
            app->voxelEditor.dynamics = VOXEL_EDIT_DYNAMIC_FLOW;
        }
    }

    nk_layout_row_dynamic(ctx, 32, 1);
    {
        
        float hz = app->voxelEditor.flowRate;
        nk_property_float(ctx, "#rate",0,&hz,255,0.01, 0.1);
        app->voxelEditor.flowRate = hz;
    }
    app_nk_gui_voxel_shape_selector(app, ctx);
    nk_layout_row_dynamic(ctx, 32 * 4, 1);
    switch (app->voxelEditor.shape)
    {
    case VOXEL_EDITOR_SHAPE_ID_POINT:
        app_nk_gui_voxel_point_props(app, ctx);
        break;
    case VOXEL_EDITOR_SHAPE_ID_CUBE:
        app_nk_gui_voxel_cube_props(app, ctx);
        break;
    case VOXEL_EDITOR_SHAPE_ID_SPHERE:
        app_nk_gui_voxel_sphere_props(app, ctx);
        break;
    }
    */
    
    
}

void app_nk_gui_camera_props(App *app, struct nk_context *ctx){
    /*
    nk_layout_row_static(ctx, 32, 64,2);

    nk_bool pred = (app->camera.cameraMode == CAMERA_MODE_PERSPECTIVE) ? nk_true : nk_false;
    if(nk_select_label(ctx, "perspective", NK_TEXT_LEFT, pred)){
        app->camera.cameraMode = CAMERA_MODE_PERSPECTIVE;
    }

    pred = (app->camera.cameraMode == CAMERA_MODE_ORTHOGRAPHIC) ? nk_true : nk_false;
    if(nk_select_label(ctx, "orthographic", NK_TEXT_LEFT, pred)){
        app->camera.cameraMode = CAMERA_MODE_ORTHOGRAPHIC;
    }

    switch (app->camera.cameraMode)
    {
    case CAMERA_MODE_PERSPECTIVE:
        {
            float fov = radToDeg(app->camera.fov);
            float near = app->camera.near ;
            float far = app->camera.far;
            
            float yaw = radToDeg(app->camera.yaw);
            float pitch = radToDeg(app->camera.pitch);
            float offsetX = app->camera.offset[0];
            float offsetY = app->camera.offset[1];
            float offsetZ = app->camera.offset[2];

            nk_layout_row_static(ctx, 32, 128,1);
            nk_property_float(ctx, "fov", 0, &fov, 180, 0.01, 0.1);
            nk_property_float(ctx, "far", near, &far, 1000, 0.01, 0.1);
            nk_property_float(ctx, "yaw", 0, &yaw, 180, 0.01, 0.1);
            nk_property_float(ctx, "pitch", 0, &pitch, 180, 0.01, 0.1);
            nk_property_float(ctx, "x", -1000, &offsetX, 1000, 0.1, 0.1);
            nk_property_float(ctx, "y", -1000, &offsetY, 1000, 0.1, 0.1);
            nk_property_float(ctx, "z", -1000, &offsetZ, 1000, 0.1, 0.1);

            app->camera.fov = degToRad(fov);
            app->camera.yaw = degToRad(yaw);
            app->camera.pitch = degToRad(pitch);
            app->camera.far = far;
            app->camera.offset[0] = offsetX;
            app->camera.offset[1] = offsetY;
            app->camera.offset[2] = offsetZ;


        }
        break;    
    case CAMERA_MODE_ORTHOGRAPHIC:
        break;
    }
    */

}

void app_nk_gui_scene_props(App *app, struct nk_context *ctx){
    /*
    float lightYaw = app->sceneLightOrientation[0];
    float lightPitch = app->sceneLightOrientation[1];
    int r = app->backgroundColor.r * 255;
    int g = app->backgroundColor.g * 255;
    int b = app->backgroundColor.b * 255;
    int a = app->backgroundColor.a * 255;
    nk_layout_row_static(ctx, 32, 128,1);
    nk_property_float(ctx, "lightYaw", 0, &lightYaw, 255, 100, 0.01);
    nk_property_float(ctx, "lightPitch", 0, &lightPitch, 255, 100, 0.01);

    nk_property_int(ctx, "r", 0, &r, 255, 1, 1);
    nk_property_int(ctx, "g", 0, &g, 255, 1, 1);
    nk_property_int(ctx, "b", 0, &b, 255, 1, 1);
    nk_property_int(ctx, "a", 0, &a, 255, 1, 1);
    {
    app->sceneLightOrientation[0] = lightYaw;
    app->sceneLightOrientation[1] = lightPitch;
    
    mat4x4  rotMat;
    vec4 dir;
    vec4 startDir = {1.0,0.0,0.0,1.0};

    mat4x4_identity(rotMat);
    mat4x4_rotate_Y(rotMat, rotMat, lightYaw);
    mat4x4_rotate_Z(rotMat, rotMat, lightPitch);
    mat4x4_mul_vec4(dir,rotMat, startDir);
    memcpy(app->sceneLightDirection, dir, sizeof(vec3));
    }
    app->backgroundColor.r = r / 255.0f;
    app->backgroundColor.g = g / 255.0f;
    app->backgroundColor.b = b / 255.0f;
    app->backgroundColor.a = a / 255.0f;
    */

}

void app_tool_bar_tool(App *app, struct nk_context *ctx, const char* text, const char* tooltip, int toolId, int *activeId){
    int pred = (activeId == toolId) ? nk_true : nk_false;
    if (nk_widget_is_hovered(ctx)){
        nk_tooltip(ctx, tooltip);


    }
    
    if(nk_select_label(ctx,text, NK_TEXT_LEFT, pred)){
        *activeId = toolId;
    }
}

void app_nk_gui_toolbar(App *app, struct nk_context *ctx){
    if(nk_group_begin(ctx, "Toolbar", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_TITLE  |NK_WINDOW_BORDER)){
        nk_layout_row_static(ctx, 48,48,2);
        app_tool_bar_tool(app, ctx, "sel", "Selection", TOOL_ID_SELECTION_TOOL, &app->activeTool);
        app_tool_bar_tool(app, ctx, "ve", "Voxel Edit", TOOL_ID_VOXEL_MANIPULATOR, &app->activeTool);
        app_tool_bar_tool(app, ctx, "mt", "Move Tool", TOOL_ID_MOVE_TOOL, &app->activeTool);
        app_tool_bar_tool(app, ctx, "bst", "Scale Tool", TOOL_ID_BOX_SCALE_TOOL, &app->activeTool);
        app_tool_bar_tool(app, ctx, "add", "Add Tool", TOOL_ID_ADD_TOOL, &app->activeTool);
        app_tool_bar_tool(app, ctx, "del", "Remove Tool", TOOL_ID_REMOVE_TOOL, &app->activeTool);
        nk_group_end(ctx);   
    }
    
}

void app_nk_gui_selector(App *app, struct nk_context *ctx, const char *text, const char *tooltip, int *propertyMenu, int selectorMenu){
    if (nk_widget_is_hovered(ctx)){
        nk_tooltip(ctx, tooltip);
    }
    int pred = (*propertyMenu == selectorMenu) ? nk_true : nk_false;
    if(nk_select_label(ctx, text, NK_TEXT_LEFT, pred)){
        *propertyMenu = selectorMenu;
    }
}

void app_nk_gui_main_window(App *app, struct nk_context *ctx){
    
    nk_style_push_float(ctx, &ctx->style.window.padding.x, 0.0);
    nk_style_push_float(ctx, &ctx->style.window.padding.y, 0.0);
    nk_style_push_float(ctx, &ctx->style.window.spacing.x, 0.0);
    nk_style_push_float(ctx, &ctx->style.window.spacing.y, 0.0);
    nk_style_push_float(ctx, &ctx->style.window.group_padding.x, 0.0);
    nk_style_push_float(ctx, &ctx->style.window.group_padding.y, 0.0);
    if(nk_begin(ctx, "App Window", nk_rect(0,0,app->windowRect.w, app->windowRect.h), 
        NK_WINDOW_BACKGROUND | NK_WINDOW_NO_SCROLLBAR |NK_WINDOW_BORDER)){
        
        app_nk_gui_menu_bar(app, ctx);

        nk_layout_row_begin(ctx, NK_STATIC,app->windowRect.h - app->menuBarHeight, 3);
        nk_layout_row_push(ctx, app->propertySelectorWidth);
        if(nk_group_begin(ctx, "Selector Panel", NK_WINDOW_NO_SCROLLBAR |NK_WINDOW_BORDER)){
            nk_layout_row_static(ctx, app->propertySelectorWidth,app->propertySelectorWidth, 1);
            app_nk_gui_selector(app, ctx, "VE","Voxel Editor Properties", 
                &app->propertyMenuId,PROPERTY_MENU_ID_VOXEL_MENU
            );
            app_nk_gui_selector(app, ctx, "CAM","Camera Properties", 
                &app->propertyMenuId,PROPERTY_MENU_ID_CAMERA_MENU
            );
            app_nk_gui_selector(app, ctx, "SCE","Editor Scene Properties", 
                &app->propertyMenuId,PROPERTY_MENU_ID_SCENE_MENU
            );

            nk_group_end(ctx);   
        }

        nk_layout_row_push(ctx, app->propertyWidth);
        if(nk_group_begin(ctx, "Side Panel", 
            NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_TITLE  |NK_WINDOW_BORDER)){
            nk_layout_row_static(ctx, (app->windowRect.h /2), 128,1);
            app_nk_gui_toolbar(app, ctx);
            
            if(nk_group_begin(ctx, "Properties Panel", NK_WINDOW_TITLE  |NK_WINDOW_BORDER)){
                switch (app->propertyMenuId)
                {
                case PROPERTY_MENU_ID_VOXEL_MENU:
                    app_nk_gui_voxel_editor_props(app, ctx);
                    break;
                case PROPERTY_MENU_ID_CAMERA_MENU:
                    app_nk_gui_camera_props(app, ctx);
                    break;
                case PROPERTY_MENU_ID_SCENE_MENU:
                    app_nk_gui_scene_props(app, ctx);
                    break;
                }
                nk_group_end(ctx);   
            }
            nk_group_end(ctx);   

        }
        

        nk_layout_row_push(ctx, app->appViewRect.w);
        if(nk_group_begin(ctx, "View Panel", NK_WINDOW_NO_SCROLLBAR |NK_WINDOW_BORDER)){
            nk_layout_row_static(ctx, app->appViewRect.h, app->appViewRect.w, 1);
            struct nk_image image = nk_image_id(app->textureId[TEXTURE_ID_APP_VIEW_TEXTURE]);
            struct nk_color white = {255,255,255,255};
            nk_image_color(ctx, image, white);

            nk_layout_row_dynamic(ctx, 32, 1);
            nk_label(ctx, app->actionBarText, NK_TEXT_LEFT);

            nk_group_end(ctx);   
        }
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    nk_style_pop_float(ctx);
    
}
