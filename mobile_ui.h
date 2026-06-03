#ifndef MOBILE_UI_H
#define MOBILE_UI_H

#include <stdbool.h>

void MobileUI_init();
void MobileUI_render(int width, int height);
void MobileUI_handleTouch(int id, float x, float y, bool down, int width, int height);

#endif
