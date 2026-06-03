#include "font.h"
#include "common.h"
#include <stdbool.h>

static void draw_segment(float x1, float y1, float x2, float y2, float x, float y, float size) {
    glVertex2f(x + x1 * size, y + y1 * size);
    glVertex2f(x + x2 * size, y + y2 * size);
}

void Font_drawChar(char c, float x, float y, float size) {
    bool s[7] = {0};
    switch (c) {
        case '0': s[0]=s[1]=s[2]=s[4]=s[5]=s[6]=1; break;
        case '1': s[2]=s[5]=1; break;
        case '2': s[0]=s[2]=s[3]=s[4]=s[6]=1; break;
        case '3': s[0]=s[2]=s[3]=s[5]=s[6]=1; break;
        case '4': s[1]=s[2]=s[3]=s[5]=1; break;
        case '5': s[0]=s[1]=s[3]=s[5]=s[6]=1; break;
        case '6': s[0]=s[1]=s[3]=s[4]=s[5]=s[6]=1; break;
        case '7': s[0]=s[2]=s[5]=1; break;
        case '8': s[0]=s[1]=s[2]=s[3]=s[4]=s[5]=s[6]=1; break;
        case '9': s[0]=s[1]=s[2]=s[3]=s[5]=s[6]=1; break;
        case 'f': s[0]=s[1]=s[3]=1; break;
        case 'p': s[0]=s[1]=s[2]=s[3]=s[4]=1; break;
        case 's': s[0]=s[1]=s[3]=s[5]=s[6]=1; break;
        case 'v': s[1]=s[4]=s[6]=s[5]=s[2]=1; break; // Rough 'v'
        case 'u': s[1]=s[4]=s[6]=s[5]=s[2]=1; break; // Rough 'u'
        case 'l': s[1]=s[4]=s[6]=1; break;
        case 'k': s[1]=s[4]=s[3]=s[2]=s[5]=1; break; // Rough 'k'
        case 'a': s[0]=s[1]=s[2]=s[3]=s[4]=s[5]=1; break;
        case 'n': s[1]=s[4]=s[0]=s[2]=s[5]=1; break;
        case '(': s[0]=s[1]=s[4]=s[6]=1; break;
        case ')': s[0]=s[2]=s[5]=s[6]=1; break;
        case '-': s[3]=1; break;
        case '.': s[6]=1; break;
    }
    glBegin(GL_LINES);
    if (s[0]) draw_segment(0, 0, 2, 0, x, y, size);
    if (s[1]) draw_segment(0, 0, 0, 2, x, y, size);
    if (s[2]) draw_segment(2, 0, 2, 2, x, y, size);
    if (s[3]) draw_segment(0, 2, 2, 2, x, y, size);
    if (s[4]) draw_segment(0, 2, 0, 4, x, y, size);
    if (s[5]) draw_segment(2, 2, 2, 4, x, y, size);
    if (s[6]) draw_segment(0, 4, 2, 4, x, y, size);
    glEnd();
}

void Font_drawString(const char* str, float x, float y, float size) {
    while (*str) {
        char c = *str;
        if (c >= 'A' && c <= 'Z') c += 32; // Lowercase
        Font_drawChar(c, x, y, size);
        x += 3.5f * size;
        str++;
    }
}
