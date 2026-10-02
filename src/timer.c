#include "timer.h"

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

static struct timespec epoch;

void game_timer_module_init(void) {
    timespec_get(&epoch, TIME_UTC);
}

uint32_t game_timer_get_ms(void) {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    long sec = now.tv_sec - epoch.tv_sec;
    long nsec = now.tv_nsec - epoch.tv_nsec;
    return 1e3 * sec + nsec / 1e6;
}

game_timer_t game_timer_create(uint32_t timeout) {
    game_timer_t timer;
    timer.timeout = timeout;
    timer.elapsed = 0;
    timer.paused = false;
    return timer;
}

void game_timer_pause(game_timer_t *timer) {
    timer->paused = true;
}

void game_timer_reset(game_timer_t *timer) {
    timer->elapsed = 0;
    timer->paused = false;
}

void game_timer_update(game_timer_t *timer, uint32_t delta) {
    if (!timer->paused) {
        timer->elapsed += delta;
    }
}

bool game_timer_has_expired(const game_timer_t *timer) {
    return timer->elapsed >= timer->timeout;
}
