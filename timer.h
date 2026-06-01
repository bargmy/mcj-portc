#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef struct {
    float ticksPerSecond;
    int64_t lastTime;
    int ticks;
    float a;
    float timeScale;
    float fps;
    float passedTime;
} Timer;

Timer Timer_create(float ticksPerSecond);
void Timer_advanceTime(Timer* self);

#endif /* TIMER_H */
