#include "timer.h"

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

static struct timespec epoch;

void init_timer_module() {
    timespec_get(&epoch, TIME_UTC);
}

uint32_t get_ms_time() {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    long sec = now.tv_sec - epoch.tv_sec;
    long nsec = now.tv_nsec - epoch.tv_nsec;
    return 1e3 * sec + nsec / 1e6;
}

tim3r_t create_timer(int timeout) {
    tim3r_t t;
    t.timeout = timeout;
    t.elapsed = 0;
    t.paused = false;
    return t;
}

void pause_timer(tim3r_t *t) {
    t->paused = true;
}

void reset_timer(tim3r_t *t) {
    t->elapsed = 0;
    t->paused = false;
}

void update_timer(tim3r_t *t, uint32_t delta) {
    if (!t->paused)
        t->elapsed += delta;
}

bool has_timed_out(const tim3r_t *t) {
    return t->elapsed >= t->timeout;
}