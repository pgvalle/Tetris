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

struct tetris_t t = {.init = false};

static void init(enum tetris_screen_t s);
static void quit(enum tetris_screen_t s);
static void process_input_event(enum tetris_screen_t s,
                                const struct tb_event *e);
static void process_frame_event(enum tetris_screen_t s, uint32_t delta);

int main() {
    init(SCREEN_PLAY);

    enum tetris_screen_t s = t.screen;
    while (true) {
        struct tb_event e;
        uint32_t start = get_ms_time();
        uint32_t timeout = MS_PER_FRAME;

        while (tb_peek_event(&e, timeout) != TB_ERR_NO_EVENT) {
            process_input_event(s, &e);
            uint32_t delta = get_ms_time() - start;
            if (delta < MS_PER_FRAME)
                timeout -= delta;
            else
                timeout = 0;
        }

        uint32_t delta = get_ms_time() - start;
        process_frame_event(s, delta);

        if (s != t.screen) {
            quit(s);
            init(t.screen);
            s = t.screen;
        }
    }

    shutdown(EXIT_SUCCESS);
}

void init(enum tetris_screen_t s) {
    if (!t.init) {
        setlocale(LC_ALL, "");
        srand(time(NULL));
        tb_init();
        init_timer_module();

        t.level = 0;
        ma_result result = ma_engine_init(NULL, &t.ma_eng);
        assert(result == MA_SUCCESS && "error starting sound engine");
        t.init = true;

        // ma_sound_init_from_file(&g.ma_eng, SFX_BASE_DIR "song1.mp3", 0, NULL,
        // NULL,
        //                         g.sounds + 0);
        // ma_sound_set_looping(g.sounds + 0, true);
        // ma_sound_init_from_file(&g.ma_eng, SFX_BASE_DIR "song2.mp3", 0, NULL,
        // NULL,
        //                         g.sounds + 1);
        // ma_sound_set_looping(g.sounds + 1, true);
        // ma_sound_init_from_file(&g.ma_eng, SFX_BASE_DIR "song3.mp3", 0, NULL,
        // NULL,
        //                         g.sounds + 2);
        // ma_sound_set_looping(g.sounds + 2, true);

        // ma_sound_start(g.sounds + 0);
    }

    switch (s) {
    case SCREEN_PLAY:
        init_play_screen();
        break;
    case SCREEN_SPLASH:
    case SCREEN_MENU:
    case SCREEN_OVER:
        break;
    }

    t.screen = s;
}

static void quit(enum tetris_screen_t s) {
    switch (s) {
    case SCREEN_PLAY:
        quit_play_screen();
        break;
    case SCREEN_SPLASH:
    case SCREEN_MENU:
    case SCREEN_OVER:
        break;
    }
}

void shutdown(int status) {
    // ma_sound_uninit(g.sounds + 0);
    // ma_sound_uninit(g.sounds + 1);
    // ma_sound_uninit(g.sounds + 2);
    ma_engine_uninit(&t.ma_eng);
    tb_shutdown();
    quit_play_screen();
    exit(status);
}

void process_input_event(enum tetris_screen_t s, const struct tb_event *e) {
    switch (s) {
    case SCREEN_PLAY:
        process_play_screen_input_event(e);
        break;
    case SCREEN_SPLASH:
    case SCREEN_MENU:
    case SCREEN_OVER:
        break;
    }
}

void process_frame_event(enum tetris_screen_t s, uint32_t delta) {
    switch (s) {
    case SCREEN_PLAY:
        process_play_screen_frame_event(delta);
        break;
    case SCREEN_SPLASH:
    case SCREEN_MENU:
    case SCREEN_OVER:
        break;
    }
}