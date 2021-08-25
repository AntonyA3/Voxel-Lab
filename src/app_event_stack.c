#include "../include/app_event_stack.h"

void app_event_stack_push(AppEventStack *eventStack, AppEvent *recentEvent){
    if(eventStack->event == NULL){
        eventStack->event = recentEvent;
    }else{
        recentEvent->prevEvent = eventStack->event;
        eventStack->event = recentEvent;
    }

}

AppEvent *app_event_stack_pop(AppEventStack *eventStack){
    if(eventStack->event != NULL){
        AppEvent* head = eventStack->event;
        eventStack->event = eventStack->event->prevEvent;
        return head;
    }
    printf("pop STACK IS EMPTY RETURN NULL\n");
    return NULL;
}