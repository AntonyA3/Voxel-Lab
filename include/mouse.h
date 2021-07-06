enum mouse_button_state{
    mouse_button_state_up, 
    mouse_button_state_pressed, 
    mouse_button_state_down, 
    mouse_button_state_released,
    mouse_button_state_count
};

enum mouse_button{
    mouse_button_left,
    mouse_button_right,
    mouse_button_count
};

typedef struct Mouse
{
    float x, y, deltaX, deltaY;
    int button[mouse_button_count];
}Mouse;

void initMouse(Mouse* mouse, float x, float y);

int nextMouseButtonState(int* mouseButtonState, int down);