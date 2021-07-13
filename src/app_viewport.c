#include "../include/app_viewport.h"

void init_app_viewport(AppViewport* appViewport, int x, int y, int width, int height){
    appViewport->x = x;
    appViewport->y = y;
    appViewport->width = width;
    appViewport->height = height;
    appViewport->ratio = width / (float)height;
}
