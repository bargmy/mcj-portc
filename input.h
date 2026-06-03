#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

typedef struct {
    bool forward;
    bool back;
    bool left;
    bool right;
    bool jump;
    
    bool attack; // Break block
    bool place;  // Place block
    
    float moveX; // For joystick -1.0 to 1.0
    float moveY; // For joystick -1.0 to 1.0
    
    float lookDX;
    float lookDY;
} InputState;

extern InputState GlobalInput;

void Input_init();
void Input_update();

#endif
