#ifndef CURSOR_H
#define CURSOR_H
#include "../include/vector2.h"
#include "../include/vector3.h"
#include "../include/ray.h"

typedef struct {
    Vector2 position;
    Vector2 clipPosition;
    Vector2 delta;
    Ray ray;
    char hit;
    int sideHit;
    Vector3 hitPosition;
}Cursor;

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

void get_clip_position_from_cursor(float *x, float *y, float cursorX, float cursorY, 
    float viewportX, float viewportY, float viewportWidth, float viewportHeight);

void get_3d_ray_from_clip_position(Ray *ray,
    float npx, float npy, float inverseMat[4][4]);
#endif