#include "bg.h"
#include "config.h"
#include "point.h"
#include "tetromino.h"
#include "timer.h"

#include <miniaudio.h>
#include <stdlib.h>
#include <termbox2.h>

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

static struct {
    enum { PLAY, SEMI_TETRIS, TETRIS } state;
    int level;
    tim3r_t gravt_tmr;
    tim3r_t decay_tmr;
    tetromino_t ttm, ttm_next;
    int ttm_stats[TETROMINO_TYPE_COUNT];
    tetromino_color_t bg[HEIGHT][WIDTH];
    ma_engine snd;
    ma_result snd_status;
    struct timespec tm_epoch;
} g;

void spawn_next_ttm();

void init();
void shutdown(int status);
void process_input_event(const struct tb_event *e);
void process_frame_event(int dt);
int get_time_ms();

int main() {
    init();

    while (true) {
        struct tb_event e;
        int start = get_time_ms();
        int timeout = MS_PER_FRAME;

        while (tb_peek_event(&e, timeout) != TB_ERR_NO_EVENT) {
            process_input_event(&e);
            int now = get_time_ms();
            timeout = MAX(MS_PER_FRAME - now + start, 0);
        }
        
        int now = get_time_ms();
        process_frame_event(now - start);
    }

    shutdown(EXIT_SUCCESS);
}

void spawn_next_ttm() {
    g.ttm = g.ttm_next;
    g.ttm.pos = (point_t){(WIDTH - 1) / 2, 0};
    g.ttm_stats[g.ttm.type]++;

    g.ttm_next = new_tetromino(15, 5);
}

void init() {
    srand(time(NULL));

    tb_init();

    g.state = PLAY;
    g.level = 0;
    g.gravt_tmr = new_timer(1000);
    g.decay_tmr = new_timer(MS_PER_FRAME * 3);

    spawn_next_ttm();
    spawn_next_ttm();
    memset(g.ttm_stats, 0, sizeof(g.ttm_stats));

    init_bg(g.bg);

    g.snd_status = ma_engine_init(NULL, &g.snd);
    if (g.snd_status != MA_SUCCESS) {
        tb_printf(0, 0, 0, 0, "You will play with no audio, unfortunately.");
        tb_printf(0, 1, 0, 0, "Press any key or move the cursor to continue.");
        tb_present();
        struct tb_event e;
        tb_poll_event(&e);
    }

    timespec_get(&g.tm_epoch, TIME_UTC);
}

int get_time_ms() {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    long sec = now.tv_sec - g.tm_epoch.tv_sec;
    long nsec = now.tv_nsec - g.tm_epoch.tv_nsec;
    return 1e3 * sec + nsec / 1e6;
}

void shutdown(int status) {
    if (g.snd_status == MA_SUCCESS) {
        ma_engine_uninit(&g.snd);
    }
    tb_shutdown();
    exit(status);
}

void process_keySTATE__event(const struct tb_event *e) {
    switch (g.state) {
    case PLAY:
        if (e->ch == 'q') {
            shutdown(EXIT_SUCCESS);
        }

        if (e->ch == 'r' || e->ch == 'R') {
            int cw = e->ch == 'R';
            rotate_tetromino(&g.ttm, cw);
            if (collide_tetromino(g.bg, &g.ttm)) {
                rotate_tetromino(&g.ttm, !cw);
            }
        }

        if (e->key == TB_KEY_ARROW_LEFT) {
            g.ttm.pos.x -= 1;
            if (collide_tetromino(g.bg, &g.ttm)) {
                g.ttm.pos.x += 1;
            }
        }
        if (e->key == TB_KEY_ARROW_RIGHT) {
            g.ttm.pos.x += 1;
            if (collide_tetromino(g.bg, &g.ttm)) {
                g.ttm.pos.x -= 1;
            }
        }
        if (e->key == TB_KEY_ARROW_DOWN) {
            g.ttm.pos.y += 1;
            reset_timer(g.gravt_tmr);
            if (collide_tetromino(g.bg, &g.ttm)) {
                g.ttm.pos.y -= 1;
                move_tetromino_to_bg(g.bg, &g.ttm);
                int a = verify_tetris(g.bg);
                if (!a)
                    spawn_next_ttm();
                else if (a == 1) {
                    ma_engine_play_sound(&g.snd, "./res/a/semi-tetris.mp3", NULL); 
                    g.state = SEMI_TETRIS;
                } else {
                    ma_engine_play_sound(&g.snd, "./res/a/tetris.mp3", NULL); 
                    g.state = TETRIS;
                }
            }
        }
        break;
    case SEMI_TETRIS:
    case TETRIS:
        break;
    }
}

void update(int dt);
void render();

void process_frame_event(int dt) {
    update(dt);
    render();
}

void process_input_event(const struct tb_event *e) {
    switch (e->type) {
    case TB_EVENT_MOUSE:
        break;
    case TB_EVENT_RESIZE:
        break;
    case TB_EVENT_KEY:
        process_key_event(e);
        break;
    default:
        break;
    }
}

void update(int dt) {
    switch (g.state) {
    case TETRIS:
        // play STATE_sound and some flashy extra visuals
    case SEMI_TETRIS: {
        // struct tb_event e;
        // tb_peek_event(&e, 5000);
        update_timer(g.decay_tmr, dt);
        if (!has_timed_out(g.decay_tmr)) {
            break;
        }

        reset_timer(g.decay_tmr);
        bool over = false;
        for (int y = 0; y < HEIGHT; y++) {
            if (g.bg[y][WIDTH - 1] != WIDTH - 1)
                continue;

            for (int x = 0; x < WIDTH - 1; x++) {
                if (g.bg[y][x] != BG_CLR) {
                    g.bg[y][x] = BG_CLR;
                    if (x == WIDTH - 2) {
                        g.bg[y][WIDTH - 1] = 0;
                        over = true;
                    }
                    break;
                }
            }
        }

        if (over) {
            compact_bg(g.bg);
            spawn_next_ttm();
            g.state = PLAY;
        }
        break;
    }
    case PLAY:
        // gravity
        update_timer(g.gravt_tmr, dt);
        if (has_timed_out(g.gravt_tmr)) {
            g.ttm.pos.y += 1;
            reset_timer(g.gravt_tmr);
            if (collide_tetromino(g.bg, &g.ttm)) {
                g.ttm.pos.y -= 1;
                move_tetromino_to_bg(g.bg, &g.ttm);
                int a = verify_tetris(g.bg);
                if (!a)
                    spawn_next_ttm();
                else if (a == 1) {
                    ma_engine_play_sound(&g.snd, "./res/a/semi-tetris.mp3", NULL); 
                    g.state = SEMI_TETRIS;
                } else {
                    ma_engine_play_sound(&g.snd, "./res/a/tetris.mp3", NULL); 
                    g.state = TETRIS;
                }
            }
        }
        break;
    }
}

void render() {
    tb_clear();
    switch (g.state) {
    case SEMI_TETRIS:
    case TETRIS:
        render_bg(g.bg);
        break;
    case PLAY:
        render_bg(g.bg);
        render_tetromino(&g.ttm);
        render_tetromino(&g.ttm_next);
        break;
    }
    tb_present();
}