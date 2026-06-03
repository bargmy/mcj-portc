#include "timer.h"
#ifdef _WIN32
#include <windows.h>
static int64_t get_nano_time() {
    LARGE_INTEGER freq;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (int64_t)(counter.QuadPart * 1000000000LL / freq.QuadPart);
}
#else
#include <time.h>
static int64_t get_nano_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}
#endif

double Util_getTime(void) {
    return (double)get_nano_time() / 1.0E9;
}

Timer Timer_create(float ticksPerSecond) {
    Timer t;
    t.ticksPerSecond = ticksPerSecond;
    t.lastTime = get_nano_time();
    t.ticks = 0;
    t.a = 0.0f;
    t.timeScale = 1.0f;
    t.fps = 0.0f;
    t.passedTime = 0.0f;
    return t;
}

void Timer_advanceTime(Timer* self) {
    if (!self) return;
    int64_t now = get_nano_time();
    int64_t passedNs = now - self->lastTime;
    self->lastTime = now;
    if (passedNs < 0LL) {
        passedNs = 0LL;
    }
    if (passedNs > 1000000000LL) {
        passedNs = 1000000000LL;
    }
    if (passedNs > 0) {
        self->fps = (float)(1000000000LL / passedNs);
    } else {
        self->fps = 0.0f;
    }
    self->passedTime += (float)passedNs * self->timeScale * self->ticksPerSecond / 1.0E9f;
    self->ticks = (int)self->passedTime;
    if (self->ticks > 100) {
        self->ticks = 100;
    }
    self->passedTime -= (float)self->ticks;
    self->a = self->passedTime;
}
