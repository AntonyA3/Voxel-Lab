#ifndef BUTTON_H
#define BUTTON_H
enum button_state{
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_DOWN,
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_UP
};

typedef struct
{
    int isDown;
    int state;
}Button;

int button_get_next_state(int state, int isDown);
#endif
