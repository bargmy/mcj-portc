#include "mobile_ui.h"
#include "common.h"
#include "input.h"
#include "font.h"
#include <math.h>

typedef struct {
    float x, y, radius;
    bool active;
    int touchId;
    float curX, curY;
} Joystick;

typedef struct {
    float x, y, w, h;
    const char* label;
    bool pressed;
    int touchId;
} Button;

static Joystick moveJoystick;
static Button jumpBtn;
static Button breakBtn;
static Button placeBtn;

void MobileUI_init() {
    moveJoystick.x = 150.0f;
    moveJoystick.y = 150.0f; // From bottom
    moveJoystick.radius = 80.0f;
    moveJoystick.active = false;
    moveJoystick.touchId = -1;

    jumpBtn.label = "JUMP";
    breakBtn.label = "BREAK";
    placeBtn.label = "PLACE";
    
    jumpBtn.touchId = -1;
    breakBtn.touchId = -1;
    placeBtn.touchId = -1;
}

static void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * (float)i / (float)num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

static void drawRect(float x, float y, float w, float h) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void MobileUI_render(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    glLineWidth(2.0f);

    // Update positions based on current width/height
    float jX = 150.0f;
    float jY = height - 150.0f;
    moveJoystick.x = jX;
    moveJoystick.y = jY;

    // Draw Joystick Base
    drawCircle(jX, jY, moveJoystick.radius, 32);
    
    // Draw Joystick Knob
    if (moveJoystick.active) {
        drawCircle(moveJoystick.curX, moveJoystick.curY, 30.0f, 16);
    } else {
        drawCircle(jX, jY, 30.0f, 16);
    }

    // Buttons
    float btnW = 120.0f;
    float btnH = 60.0f;
    
    jumpBtn.x = width - 150.0f;
    jumpBtn.y = height - 100.0f;
    jumpBtn.w = btnW;
    jumpBtn.h = btnH;

    breakBtn.x = width - 150.0f;
    breakBtn.y = height - 200.0f;
    breakBtn.w = btnW;
    breakBtn.h = btnH;

    placeBtn.x = width - 150.0f;
    placeBtn.y = height - 300.0f;
    placeBtn.w = btnW;
    placeBtn.h = btnH;

    // Draw Buttons
    if (jumpBtn.pressed) glColor4f(0.5f, 1.0f, 0.5f, 0.8f); else glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    drawRect(jumpBtn.x, jumpBtn.y, jumpBtn.w, jumpBtn.h);
    Font_drawString(jumpBtn.label, jumpBtn.x + 10, jumpBtn.y + 15, 3.0f);

    if (breakBtn.pressed) glColor4f(1.0f, 0.5f, 0.5f, 0.8f); else glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    drawRect(breakBtn.x, breakBtn.y, breakBtn.w, breakBtn.h);
    Font_drawString(breakBtn.label, breakBtn.x + 10, breakBtn.y + 15, 3.0f);

    if (placeBtn.pressed) glColor4f(0.5f, 0.5f, 1.0f, 0.8f); else glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    drawRect(placeBtn.x, placeBtn.y, placeBtn.w, placeBtn.h);
    Font_drawString(placeBtn.label, placeBtn.x + 10, placeBtn.y + 15, 3.0f);

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

static bool inRect(float px, float py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

void MobileUI_handleTouch(int id, float x, float y, bool down, int width, int height) {
    if (down) {
        // Check joystick
        float dx = x - moveJoystick.x;
        float dy = y - moveJoystick.y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < moveJoystick.radius * 1.5f && !moveJoystick.active) {
            moveJoystick.active = true;
            moveJoystick.touchId = id;
            moveJoystick.curX = x;
            moveJoystick.curY = y;
        }

        // Check buttons
        if (inRect(x, y, jumpBtn.x, jumpBtn.y, jumpBtn.w, jumpBtn.h)) {
            jumpBtn.pressed = true;
            jumpBtn.touchId = id;
            GlobalInput.jump = true;
        }
        if (inRect(x, y, breakBtn.x, breakBtn.y, breakBtn.w, breakBtn.h)) {
            breakBtn.pressed = true;
            breakBtn.touchId = id;
            GlobalInput.attack = true;
        }
        if (inRect(x, y, placeBtn.x, placeBtn.y, placeBtn.w, placeBtn.h)) {
            placeBtn.pressed = true;
            placeBtn.touchId = id;
            GlobalInput.place = true;
        }
    } else {
        // Touch up
        if (id == moveJoystick.touchId) {
            moveJoystick.active = false;
            moveJoystick.touchId = -1;
            GlobalInput.moveX = 0;
            GlobalInput.moveY = 0;
        }
        if (id == jumpBtn.touchId) { jumpBtn.pressed = false; jumpBtn.touchId = -1; GlobalInput.jump = false; }
        if (id == breakBtn.touchId) { breakBtn.pressed = false; breakBtn.touchId = -1; GlobalInput.attack = false; }
        if (id == placeBtn.touchId) { placeBtn.pressed = false; placeBtn.touchId = -1; GlobalInput.place = false; }
    }

    if (down && id == moveJoystick.touchId) {
        float dx = x - moveJoystick.x;
        float dy = y - moveJoystick.y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist > moveJoystick.radius) {
            dx = (dx / dist) * moveJoystick.radius;
            dy = (dy / dist) * moveJoystick.radius;
        }
        moveJoystick.curX = moveJoystick.x + dx;
        moveJoystick.curY = moveJoystick.y + dy;
        
        GlobalInput.moveX = dx / moveJoystick.radius;
        GlobalInput.moveY = dy / moveJoystick.radius;
    }
}
