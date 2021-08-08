#ifndef APP_NK_GUI_H
#define APP_NK_GUI_H
#include "../include/app.h"

#ifndef NK_GUI_HEADER
#define NK_GUI_HEADER
#include "../nuklear/nuklear.h"
#endif
#include "voxel_edit_dynamic_id.h"
#include "property_menu_id.h"

void app_nk_gui_menu_bar(App *app, struct nk_context *ctx);
void app_nk_gui_main_window(App *app, struct nk_context *ctx);
void app_nk_gui_voxel_point_props(App *app, struct nk_context *ctx);
void app_nk_gui_voxel_cube_props(App *app, struct nk_context *ctx);
void app_nk_gui_voxel_editor_props(App *app, struct nk_context *ctx);
void app_nk_gui_voxel_sphere_props(App *app, struct nk_context *ctx);
void app_nk_gui_camera_props(App *app, struct nk_context *ctx);
void app_nk_gui_scene_props(App *app, struct nk_context *ctx);

#endif