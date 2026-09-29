#ifndef TETRIS_TIMER_H
#define TETRIS_TIMER_H

typedef struct {
    int timeout;
    int delta;
} tim3r_t;

#define new_timer(timeout) ((tim3r_t) {timeout, 0})
#define reset_timer(t) ((t).delta = 0)
#define update_timer(t, dt) ((t).delta += dt)
#define has_timed_out(t) ((t).delta >= (t).timeout)

#endif // TETRIS_TIMER_H