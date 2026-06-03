#include "input.h"
#include <string.h>

InputState GlobalInput;

void Input_init() {
    memset(&GlobalInput, 0, sizeof(InputState));
}

void Input_update() {
    // This will be called to reset transient inputs like lookDX/DY 
    // or handle periodic updates if needed.
    GlobalInput.lookDX = 0;
    GlobalInput.lookDY = 0;
}
