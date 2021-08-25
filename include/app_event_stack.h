#ifndef APP_EVENT_STACK_H
#define APP_EVENT_STACK_H
#include <stdlib.h>
#include "../include/app_event.h"

enum app_event{
    APP_EVENT_ADD_VOXEL_AABB,
    APP_EVENT_DELETE_VOXEL_AABB
};

typedef struct
{
    AppEvent *event;
}AppEventStack;

void app_event_stack_push(AppEventStack *eventStack, AppEvent *recentEvent);
AppEvent *app_event_stack_pop(AppEventStack *eventStack);
#endif