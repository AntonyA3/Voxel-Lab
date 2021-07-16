#ifndef NUKLEAR_GUI_H
#define NUKLEAR_GUI_H

#ifndef NUKLEAR_H
#define NUKLEAR_H
#include "../nuklear/nuklear.h"
#endif

#include "../include/app.h"

void do_show_cursor_debug(struct nk_context *ctx, App app);
void do_show_orbit_camera_debug(struct nk_context *ctx, App app);
void do_show_floor_grid_debug(struct nk_context *ctx, App app);
void do_show_voxel_debug(struct nk_context *ctx, App app);

void do_show_debug_window(struct nk_context *ctx, App app);
void do_show_file_menu(struct nk_context *ctx, App *app);
void do_show_menu_bar(struct nk_context *ctx, App *app);
void do_show_main_window(struct nk_context *ctx, App *app);
void do_show_property_selector_panel(struct nk_context *ctx, App *app);
void do_show_properties_panel(struct nk_context *ctx, App *app);
void do_show_render_panel(struct nk_context *ctx, App *app);
void do_show_main_window(struct nk_context *ctx, App *app);

#endif