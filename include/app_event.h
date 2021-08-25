#ifndef APP_EVENT_H
#define APP_EVENT_H
typedef struct AppEvent
{
    int eventType;
    void *eventData;
    struct AppEvent *prevEvent;
}AppEvent;
#endif
