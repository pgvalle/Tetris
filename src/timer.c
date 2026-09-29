#include "timer.h"
#include "config.h"

#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
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
    t.epoch = get_ms_time();
    t.paused = false;
    return t;
}

void pause_timer(tim3r_t *t) {
    t->paused = true;
}

void reset_timer(tim3r_t *t) {
    t->epoch = get_ms_time();
    t->paused = false;
}

bool has_timed_out(const tim3r_t *t) {
    uint32_t now = get_ms_time();
    return now - t->epoch >= t->timeout;
}