#include "../include/button.h"

int button_get_next_state(int state, int isDown){
    switch (state)
    {
    case BUTTON_STATE_DOWN:
        if(!isDown){
            return BUTTON_STATE_RELEASED;
        }           
        break;
    case BUTTON_STATE_PRESSED:
        if(isDown){
            return BUTTON_STATE_DOWN;
        }else{
            return BUTTON_STATE_RELEASED;
        }
        break;
    case BUTTON_STATE_RELEASED:
        if(isDown){
            return BUTTON_STATE_PRESSED;
        }else{
            return BUTTON_STATE_UP;
        }
        break;
    case BUTTON_STATE_UP:
        if(isDown){
            return BUTTON_STATE_PRESSED;
        }
        break;
    }
}