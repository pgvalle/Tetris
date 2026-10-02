#ifndef TETRIS_TIMER_H
#define TETRIS_TIMER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t timeout;
    uint32_t elapsed;
    bool paused;
} game_timer_t;

void game_timer_module_init(void);
uint32_t game_timer_get_ms(void);

game_timer_t game_timer_create(uint32_t timeout);
void game_timer_pause(game_timer_t *timer);
// unpauses it too
void game_timer_reset(game_timer_t *timer);
void game_timer_update(game_timer_t *timer, uint32_t delta);
bool game_timer_has_expired(const game_timer_t *timer);

#endif // TETRIS_TIMER_H
