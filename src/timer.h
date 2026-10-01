#ifndef TETRIS_TIMER_H
#define TETRIS_TIMER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t timeout;
    uint32_t elapsed;
    bool paused;
} tim3r_t;

void init_timer_module();
uint32_t get_ms_time();

tim3r_t create_timer(int timeout);
void pause_timer(tim3r_t *t);
// unpauses it too
void reset_timer(tim3r_t *t);
void update_timer(tim3r_t *t, uint32_t delta);
bool has_timed_out(const tim3r_t *t);

#endif // TETRIS_TIMER_H