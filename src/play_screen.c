#include "background.h"
#include "tetris.h"
#include "tetromino.h"
#include "timer.h"

#include <termbox2.h>

#include <stdint.h>

#define WIDTH2 ((WIDTH - 1) / 2)

static struct {
    enum { STATE_PLAY, STATE_SEMI_TETRIS, STATE_TETRIS } state;
    tetromino_color_t bg[HEIGHT][WIDTH];
    tetromino_t ttm, next_ttm;
    uint32_t ttm_statistics[TETROMINO_TYPE_COUNT];
    tim3r_t gravt_tmr;
    tim3r_t decay_tmr;
    bool decay_highlight;
    int lines;
    bool pause;
} p;

static void spawn_next_ttm();
static void update();
static void render();
static void render_background_frame(int xoff, int yoff);
static void render_next_tetromino(int xoff, int yoff);
static void render_tetromino_statistics(int xoff, int yoff);
static void render_lines(int xoff, int yoff);

void init_play_screen() {
    p.state = STATE_PLAY;
    init_background(p.bg);
    p.ttm = create_tetromino(WIDTH2, 0);
    p.next_ttm = create_tetromino(0, 0);
    memset(p.ttm_statistics, 0, sizeof(p.ttm_statistics));
    p.ttm_statistics[p.ttm.type]++;
    p.gravt_tmr = create_timer(800);
    p.decay_tmr = create_timer(80);
    p.decay_highlight = false;
    p.lines = 0;
    p.pause = false;
}

void quit_play_screen() {
}

void process_play_screen_input_event(const struct tb_event *e) {
    if (e->type != TB_EVENT_KEY)
        return;

    if (e->ch == 'p') {
        p.pause = !p.pause;
        p.gravt_tmr.paused = p.pause;
        p.decay_tmr.paused = p.pause;
        return;
    } else if (e->ch == 'q') {
        shutdown(EXIT_SUCCESS);
    }

    switch (p.state) {
    case STATE_PLAY:
        if (p.pause)
            break;

        switch (e->ch) {
        case 'x':
            rotate_tetromino(&p.ttm, 1);
            if (collide_tetromino(p.bg, &p.ttm)) {
                rotate_tetromino(&p.ttm, 0);
            } else {
                // ma_engine_play_sound(&p.ma_eng, SFX_BASE_DIR "rotation.mp3",
                //                      NULL);
            }
            break;
        case 'z':
            rotate_tetromino(&p.ttm, 0);
            if (collide_tetromino(p.bg, &p.ttm)) {
                rotate_tetromino(&p.ttm, 1);
            } else {
                // ma_engine_play_sound(&g.ma_eng, SFX_BASE_DIR "rotation.mp3",
                //                      NULL);
            }
            break;
        }

        point_t prev_pos = p.ttm.pos;
        p.ttm.pos.x += e->key == TB_KEY_ARROW_RIGHT ? 1 : 0;
        p.ttm.pos.x -= e->key == TB_KEY_ARROW_LEFT ? 1 : 0;
        p.ttm.pos.y += e->key == TB_KEY_ARROW_DOWN ? 1 : 0;
        bool moved_down = p.ttm.pos.y != prev_pos.y;
        bool moved_sideways = p.ttm.pos.x != prev_pos.x;

        if (collide_tetromino(p.bg, &p.ttm)) {
            p.ttm.pos = prev_pos;
            if (moved_down) {
                reset_timer(&p.gravt_tmr);
                move_tetromino_to_background(p.bg, &p.ttm);
                int tetris = verify_tetris(p.bg);
                if (!tetris) {
                    // ma_engine_play_sound(&g.ma_eng, SFX_BASE_DIR
                    // "placed.mp3",
                    //                      NULL);
                    spawn_next_ttm();
                } else if (tetris == 1) {
                    // ma_engine_play_sound(&g.ma_eng,
                    //                      SFX_BASE_DIR "semi-tetris.mp3",
                    //                      NULL);
                    p.state = STATE_SEMI_TETRIS;
                    reset_timer(&p.decay_tmr);
                } else {
                    // ma_engine_play_sound(&g.ma_eng, SFX_BASE_DIR
                    // "tetris.mp3",
                    //                      NULL);
                    p.state = STATE_TETRIS;
                    p.decay_highlight = true;
                    reset_timer(&p.decay_tmr);
                }
            }
        } else if (moved_sideways) {
            // ma_engine_play_sound(&g.ma_eng, SFX_BASE_DIR "sideways.mp3",
            // NULL);
        }
        break;
    case STATE_SEMI_TETRIS:
    case STATE_TETRIS:
        break;
    }
}

void process_play_screen_frame_event(uint32_t delta) {
    update_timer(&p.gravt_tmr, delta);
    update_timer(&p.decay_tmr, delta);
    update();
    render();
}

static void spawn_next_ttm() {
    p.ttm = p.next_ttm;
    p.ttm.pos = create_point(WIDTH2, 0);
    p.next_ttm = create_tetromino(0, 0);
    p.ttm_statistics[p.ttm.type]++;
}

static void update() {
    switch (p.state) {
    case STATE_TETRIS:
    case STATE_SEMI_TETRIS:
        if (!has_timed_out(&p.decay_tmr))
            break;
        if (p.state == STATE_TETRIS)
            p.decay_highlight = !p.decay_highlight;

        reset_timer(&p.decay_tmr);
        for (int y = 0; y < HEIGHT; y++) {
            if (p.bg[y][WIDTH - 1] != WIDTH - 1)
                continue;

            for (int x = 0; x < WIDTH2; x++) {
                if (p.bg[y][WIDTH2 + x] == BG_CLR)
                    continue;

                p.bg[y][WIDTH2 + x] = BG_CLR;
                p.bg[y][WIDTH2 - x - 1] = BG_CLR;
                if (x == WIDTH2 - 1) {
                    p.state = STATE_PLAY;
                    p.bg[y][WIDTH - 1] = 0;
                    p.lines++;
                    p.decay_highlight = false;
                }
                break;
            }
        }

        if (p.state == STATE_PLAY) {
            compact_background(p.bg);
            spawn_next_ttm();
        }
        break;
    case STATE_PLAY:
        if (p.pause)
            break;
        if (!has_timed_out(&p.gravt_tmr))
            break;

        p.ttm.pos.y += 1;
        reset_timer(&p.gravt_tmr);
        if (!collide_tetromino(p.bg, &p.ttm))
            break;

        p.ttm.pos.y -= 1;
        move_tetromino_to_background(p.bg, &p.ttm);

        int tetris = verify_tetris(p.bg);
        if (!tetris) {
            // ma_engine_play_sound(&p.ma_eng, SFX_BASE_DIR "placed.mp3",
            //                      NULL);
            spawn_next_ttm();
        } else if (tetris == 1) {
            // ma_engine_play_sound(&g.ma_eng,
            //                      SFX_BASE_DIR "semi-tetris.mp3", NULL);
            p.state = STATE_SEMI_TETRIS;
            reset_timer(&p.decay_tmr);
        } else {
            // ma_engine_play_sound(&g.ma_eng, SFX_BASE_DIR "tetris.mp3",
            //                      NULL);
            p.state = STATE_TETRIS;
            p.decay_highlight = true;
            reset_timer(&p.decay_tmr);
        }

        break;
    }
}

static void render() {
    tb_clear();
    switch (p.state) {
    case STATE_PLAY:
        if (!p.pause)
            render_background(17, 1, p.bg);
        else
            render_background_highlighted(17, 1, p.bg);
        render_tetromino(17, 1, &p.ttm);
        render_next_tetromino(1, 3);
        break;
    case STATE_SEMI_TETRIS:
        render_background(17, 1, p.bg);
        render_next_tetromino(1, 3);
        break;
    case STATE_TETRIS:
        if (!p.decay_highlight)
            render_background(17, 1, p.bg);
        else
            render_background_highlighted(17, 1, p.bg);
        render_next_tetromino(1, 3);
        break;
    }

    render_background_frame(16, 0);
    render_lines(1, 0);
    render_tetromino_statistics(1, 7);
    if (p.pause)
        tb_printf(24, 10, 0, 0, "PAUSED");
    tb_present();
}

static void render_background_frame(int xoff, int yoff) {
    tb_printf(xoff, yoff, 0, 0, TL HH HH HH HH HH HH HH HH HH HH TR);
    tb_printf(xoff, yoff + HEIGHT + 1, 0, 0,
              BL HH HH HH HH HH HH HH HH HH HH BR);
    for (int y = 0; y < HEIGHT; y++) {
        tb_printf(xoff, yoff + y + 1, 0, 0, V);
        tb_printf(xoff + 21, yoff + y + 1, 0, 0, V);
    }
}

static void render_next_tetromino(int xoff, int yoff) {
    tb_printf(xoff, yoff + 0, 0, 0, TL HH H " NEXT " H HH TR);
    tb_printf(xoff, yoff + 1, 0, 0, V "            " V);
    tb_printf(xoff, yoff + 2, 0, 0, V "            " V);
    tb_printf(xoff, yoff + 3, 0, 0, BL HH HH HH HH HH HH BR);

    switch (p.next_ttm.type) {
    case TETROMINO_TYPE_I:
    case TETROMINO_TYPE_O:
        render_tetromino(xoff + 7, yoff + 1, &p.next_ttm);
        break;
    default:
        render_tetromino(xoff + 6, yoff + 1, &p.next_ttm);
    }
}

static void render_tetromino_statistics(int xoff, int yoff) {
    for (int i = 0; i < TETROMINO_TYPE_COUNT; i++) {
        const char *utf8 = get_tetromino_1x4_utf8(i);
        uint32_t stats = p.ttm_statistics[i];
        tb_printf(xoff, 2 * i + yoff + 1, 0, 0, V " %s %05d " V, utf8, stats);
        tb_printf(xoff, 2 * i + yoff + 2, 0, 0, V "            " V);
    }

    tb_printf(xoff, yoff + 0, 0, 0, TL " STATISTICS " TR);
    tb_printf(xoff, yoff + 14, 0, 0, BL HH HH HH HH HH HH BR);
}

static void render_lines(int xoff, int yoff) {
    tb_printf(xoff, yoff + 0, 0, 0, TL HH HH HH HH HH HH TR);
    tb_printf(xoff, yoff + 1, 0, 0, V " LINES %04d " V, p.lines);
    tb_printf(xoff, yoff + 2, 0, 0, BL HH HH HH HH HH HH BR);
}
