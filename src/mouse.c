#include "../include/mouse.h"

void initMouse(Mouse* mouse, float x, float y){
    mouse->x = x;
    mouse->y = y;
    mouse->deltaX = 0;
    mouse->deltaY = 0;
    for(int i = 0; i < mouse_button_count; i++){
        mouse->button[i] = mouse_button_state_up;
    }
}

int nextMouseButtonState(int* mouseButtonState, int down){
    switch (*mouseButtonState)
    {
    case mouse_button_state_down:
        *mouseButtonState = !down * mouse_button_state_released + down * mouse_button_state_down;
        break;
    case mouse_button_state_pressed:
        *mouseButtonState = !down * mouse_button_state_released + down * mouse_button_state_down;
        break;
    case mouse_button_state_released:
        *mouseButtonState = !down * mouse_button_state_up + down * mouse_button_state_pressed;
        break;
    case mouse_button_state_up:
        *mouseButtonState = !down * mouse_button_state_up + down * mouse_button_state_pressed;
        break;
    }
}