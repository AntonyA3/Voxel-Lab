
enum cursor_button_state{
    CURSOR_BUTTON_STATE_UP,
    CURSOR_BUTTON_STATE_PRESSED,
    CURSOR_BUTTON_STATE_DOWN,
    CURSOR_BUTTON_STATE_RELEASED
};

enum cursor_button_id{
    CURSOR_BUTTON_LEFT,
    CURSOR_BUTTON_RIGHT,
    CURSOR_BUTTON_COUNT
};

int cursor_button_next_state(int state, int isDown);

void get_norm_position_from_cursor(float *x, float *y, float cursorX, float cursorY, 
    float viewportX, float viewportY, float viewportWidth, float viewportHeight);

void get_3d_ray_from_norm_position(float *x, float *y, float *z, float *dx, float*dy, float*dz,
    float npx, float npy, float inverseMat[4][4]);