#include "config.h"
#include "tetris.h"
#include "timer.h"

#include <miniaudio.h>
#include <stdlib.h>
#include <termbox2.h>

#include <assert.h>
#include <locale.h>
#include <stdbool.h>
#include <time.h>

/*
Gameplay state machine
Play
 - move/rotate pieces
 fill 4 rows -> TETRIS
 fill 1-3 rows -> SEMITETRIS
TETRIS
 - animation on decaying rows
 - a sound
 -> PLAY
SEMITETRIS
 - animation on decaying rows
 - another sound
 -> PLAY
*/

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MS_PER_FRAME (1000 / FPS)

struct tetris_t g_game = {.init = false};

static void init(enum tetris_screen_t s);
static void quit(enum tetris_screen_t s);
static void process_input_event(enum tetris_screen_t s,
                                const struct tb_event *e);
static void process_frame_event(enum tetris_screen_t s, uint32_t delta);

int main(void) {
    init(SCREEN_PLAY);

    enum tetris_screen_t s = g_game.screen;
    while (true) {
        struct tb_event e;
        uint32_t start = game_timer_get_ms();
        uint32_t timeout = MS_PER_FRAME;

        while (tb_peek_event(&e, timeout) != TB_ERR_NO_EVENT) {
            process_input_event(s, &e);
            uint32_t delta = game_timer_get_ms() - start;
            if (delta < MS_PER_FRAME) {
                timeout -= delta;
            } else {
                timeout = 0;
            }
        }

        uint32_t delta = game_timer_get_ms() - start;
        process_frame_event(s, delta);

        if (s != g_game.screen) {
            quit(s);
            init(g_game.screen);
            s = g_game.screen;
        }
    }

    shutdown_game(EXIT_SUCCESS);
}

static void init(enum tetris_screen_t s) {
    if (!g_game.init) {
        setlocale(LC_ALL, "");
        srand(time(NULL));
        tb_init();
        game_timer_module_init();

        g_game.level = 0;
        ma_result result = ma_engine_init(NULL, &g_game.ma_eng);
        assert(result == MA_SUCCESS && "error starting sound engine");
        g_game.init = true;
    }

    switch (s) {
    case SCREEN_PLAY:
        play_screen_init();
        break;
    case SCREEN_SPLASH:
    case SCREEN_MENU:
    case SCREEN_OVER:
        break;
    }

    g_game.screen = s;
}

static void quit(enum tetris_screen_t s) {
    switch (s) {
    case SCREEN_PLAY:
        play_screen_quit();
        break;
    case SCREEN_SPLASH:
    case SCREEN_MENU:
    case SCREEN_OVER:
        break;
    }
}

void shutdown_game(int status) {
    ma_engine_uninit(&g_game.ma_eng);
    tb_shutdown();
    play_screen_quit();
    exit(status);
}

static void process_input_event(enum tetris_screen_t s, const struct tb_event *e) {
    switch (s) {
    case SCREEN_PLAY:
        play_screen_process_input(e);
        break;
    case SCREEN_SPLASH:
    case SCREEN_MENU:
    case SCREEN_OVER:
        break;
    }
}

static void process_frame_event(enum tetris_screen_t s, uint32_t delta) {
    switch (s) {
    case SCREEN_PLAY:
        play_screen_process_frame(delta);
        break;
    case SCREEN_SPLASH:
    case SCREEN_MENU:
    case SCREEN_OVER:
        break;
    }
}
