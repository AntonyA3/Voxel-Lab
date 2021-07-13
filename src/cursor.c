#include "../include/cursor.h"

int cursor_button_next_state(int state, int isDown){
    switch (state)
    {
    case CURSOR_BUTTON_STATE_UP:
        return isDown * CURSOR_BUTTON_STATE_PRESSED + !isDown * state;
        break;
    case CURSOR_BUTTON_STATE_PRESSED:
        return isDown * CURSOR_BUTTON_STATE_DOWN + !isDown * CURSOR_BUTTON_STATE_RELEASED;
        break;

    case CURSOR_BUTTON_STATE_DOWN:
        return isDown * state + !isDown * CURSOR_BUTTON_STATE_RELEASED;
        break;

    case CURSOR_BUTTON_STATE_RELEASED:
        return isDown * CURSOR_BUTTON_STATE_PRESSED + !isDown * CURSOR_BUTTON_STATE_UP;
        break;

    }
}

void get_norm_position_from_cursor(float *x, float *y, float cursorX, float cursorY, 
    float viewportX, float viewportY, float viewportWidth, float viewportHeight){
        *x = 2* (cursorX - viewportX) / viewportWidth -1;
        *y = -2 *(cursorY - viewportY) / viewportHeight + 1;
}
void get_3d_ray_from_norm_position(float *x, float *y, float *z, float *dx, float*dy, float*dz,
    float npx, float npy, float inverseMat[4][4]){

}