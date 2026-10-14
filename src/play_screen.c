#include "background.h"
#include "config.h"
#include "tetris.h"
#include "tetromino.h"
#include "timer.h"

#include <termbox2.h>

#include <stdint.h>
#include <string.h>

#define WIDTH2 ((WIDTH - 1) / 2)

static struct {
    enum { STATE_PLAY, STATE_SEMI_TETRIS, STATE_TETRIS, STATE_OVER } state;
    tetromino_color_t bg[HEIGHT][WIDTH];
    tetromino_t current_tetromino, next_tetromino;
    uint32_t ttm_statistics[TETROMINO_TYPE_COUNT];
    game_timer_t gravity_timer;
    game_timer_t decay_timer;
    bool decay_highlight;
    int lines;
    bool pause;
    int bonus;
} s_play_screen;

static bool check_and_set_game_over();
static void check_and_set_tetris();
static void spawn_next_tetromino(void);
static void update(void);
static void render(void);
static void render_score(int xoff, int yoff);
static void render_background_frame(int xoff, int yoff);
static void render_next_tetromino(int xoff, int yoff);
static void render_tetromino_statistics(int xoff, int yoff);
static void render_lines(int xoff, int yoff);

void play_screen_init(void) {
    s_play_screen.state = STATE_PLAY;
    background_init(s_play_screen.bg);
    s_play_screen.current_tetromino = tetromino_create(WIDTH2, 0);
    s_play_screen.next_tetromino = tetromino_create(0, 0);
    memset(s_play_screen.ttm_statistics, 0, sizeof(s_play_screen.ttm_statistics));
    s_play_screen.ttm_statistics[s_play_screen.current_tetromino.type]++;
    s_play_screen.gravity_timer = game_timer_create(800);
    s_play_screen.decay_timer = game_timer_create(80);
    s_play_screen.decay_highlight = false;
    s_play_screen.lines = 0;
    s_play_screen.pause = false;
    s_play_screen.bonus = 0;
}

void play_screen_quit(void) {
}

void play_screen_process_input(const struct tb_event *e) {
    if (e->type != TB_EVENT_KEY)
        return;

    if (e->ch == 'q') {
        shutdown_game(EXIT_SUCCESS);
    }

    switch (s_play_screen.state) {
    case STATE_PLAY:
        if (e->ch == 'p') {
            s_play_screen.pause = !s_play_screen.pause;
            s_play_screen.gravity_timer.paused = s_play_screen.pause;
            s_play_screen.decay_timer.paused = s_play_screen.pause;
            break;
        }

        if (s_play_screen.pause)
            break;
        if (check_and_set_game_over())
            break;

        switch (e->ch) {
        case 'x':
            tetromino_rotate(&s_play_screen.current_tetromino, true);
            if (background_check_collision(s_play_screen.bg, &s_play_screen.current_tetromino)) {
                tetromino_rotate(&s_play_screen.current_tetromino, false);
            }
            break;
        case 'z':
            tetromino_rotate(&s_play_screen.current_tetromino, false);
            if (background_check_collision(s_play_screen.bg, &s_play_screen.current_tetromino)) {
                tetromino_rotate(&s_play_screen.current_tetromino, true);
            }
            break;
        }

        point_t prev_pos = s_play_screen.current_tetromino.pos;
        s_play_screen.current_tetromino.pos.x += e->key == TB_KEY_ARROW_RIGHT ? 1 : 0;
        s_play_screen.current_tetromino.pos.x -= e->key == TB_KEY_ARROW_LEFT ? 1 : 0;
        s_play_screen.current_tetromino.pos.y += e->key == TB_KEY_ARROW_DOWN ? 1 : 0;
        bool moved_down = s_play_screen.current_tetromino.pos.y != prev_pos.y;
        bool moved_sideways = s_play_screen.current_tetromino.pos.x != prev_pos.x;

        if (moved_down)
            s_play_screen.bonus += 1;

        if (background_check_collision(s_play_screen.bg, &s_play_screen.current_tetromino)) {
            s_play_screen.current_tetromino.pos = prev_pos;
            if (moved_down) {
                game_timer_reset(&s_play_screen.gravity_timer);
                background_place_tetromino(s_play_screen.bg, &s_play_screen.current_tetromino);
                check_and_set_tetris();
            }
        } else if (moved_sideways) {
            // sideways move
        }
        break;
    case STATE_SEMI_TETRIS:
    case STATE_TETRIS:
        if (e->ch == 'p') {
            s_play_screen.pause = !s_play_screen.pause;
            s_play_screen.gravity_timer.paused = s_play_screen.pause;
            s_play_screen.decay_timer.paused = s_play_screen.pause;
        }
        break;
    case STATE_OVER:
        break;
    }
}

void play_screen_process_frame(uint32_t delta) {
    game_timer_update(&s_play_screen.gravity_timer, delta);
    game_timer_update(&s_play_screen.decay_timer, delta);
    update();
    render();
}

static bool check_and_set_game_over() {
    bool over = s_play_screen.current_tetromino.pos.y == 0 &&
                background_check_collision(s_play_screen.bg, &s_play_screen.current_tetromino);
    if (over) {
        s_play_screen.state = STATE_OVER;
        background_place_tetromino(s_play_screen.bg, &s_play_screen.current_tetromino);
        game_timer_reset(&s_play_screen.decay_timer);
        for (int y = 0; y < HEIGHT; y++)
            s_play_screen.bg[y][WIDTH - 1] = 0;
    }
    return over;
}

static void check_and_set_tetris() {
    int tetris = background_verify_tetris(s_play_screen.bg);
    if (!tetris) {
        g_game.score += s_play_screen.bonus;
        s_play_screen.bonus = 0;
        spawn_next_tetromino();
    } else if (tetris == 1) {
        s_play_screen.state = STATE_SEMI_TETRIS;
        game_timer_reset(&s_play_screen.decay_timer);
    } else {
        s_play_screen.state = STATE_TETRIS;
        s_play_screen.decay_highlight = true;
        game_timer_reset(&s_play_screen.decay_timer);
    }
}

static void spawn_next_tetromino(void) {
    s_play_screen.current_tetromino = s_play_screen.next_tetromino;
    s_play_screen.current_tetromino.pos = point_create(WIDTH2, 0);
    s_play_screen.next_tetromino = tetromino_create(0, 0);
    s_play_screen.ttm_statistics[s_play_screen.current_tetromino.type]++;
}

static void update(void) {
    switch (s_play_screen.state) {
    case STATE_TETRIS:
    case STATE_SEMI_TETRIS:
        if (s_play_screen.pause)
            break;
        if (!game_timer_has_expired(&s_play_screen.decay_timer))
            break;

        if (s_play_screen.state == STATE_TETRIS)
            s_play_screen.decay_highlight = !s_play_screen.decay_highlight;

        int lines = s_play_screen.lines;

        game_timer_reset(&s_play_screen.decay_timer);
        for (int y = 0; y < HEIGHT; y++) {
            if (s_play_screen.bg[y][WIDTH - 1] != WIDTH - 1)
                continue;

            for (int x = 0; x < WIDTH2; x++) {
                if (s_play_screen.bg[y][WIDTH2 + x] == BG_CLR)
                    continue;

                s_play_screen.bg[y][WIDTH2 + x] = BG_CLR;
                s_play_screen.bg[y][WIDTH2 - x - 1] = BG_CLR;
                if (x == WIDTH2 - 1) {
                    s_play_screen.state = STATE_PLAY;
                    s_play_screen.bg[y][WIDTH - 1] = 0;
                    s_play_screen.lines++;
                    s_play_screen.decay_highlight = false;
                }
                break;
            }
        }

        if (s_play_screen.state == STATE_PLAY) {
            static const int values[] = {40, 100, 300, 1200};
            int diff = s_play_screen.lines - lines;
            g_game.score += values[diff - 1] * (g_game.level + 1) + s_play_screen.bonus;
            s_play_screen.bonus = 0;
            background_compact(s_play_screen.bg);
            spawn_next_tetromino();
        }
        break;
    case STATE_PLAY:
        if (s_play_screen.pause)
            break;
        if (!game_timer_has_expired(&s_play_screen.gravity_timer))
            break;
        if (check_and_set_game_over())
            break;

        s_play_screen.current_tetromino.pos.y += 1;
        game_timer_reset(&s_play_screen.gravity_timer);
        if (!background_check_collision(s_play_screen.bg, &s_play_screen.current_tetromino))
            break;

        s_play_screen.current_tetromino.pos.y -= 1;
        background_place_tetromino(s_play_screen.bg, &s_play_screen.current_tetromino);
        check_and_set_tetris();
        break;
    case STATE_OVER:
        if (!game_timer_has_expired(&s_play_screen.decay_timer))
            break;

        if (s_play_screen.bg[HEIGHT - 1][WIDTH - 1]) {
            // g_game.screen = SCREEN_PLAY;
            play_screen_quit();
            play_screen_init();
            break;
        }

        game_timer_reset(&s_play_screen.decay_timer);
        for (int y = 0; y < HEIGHT; y++) {
            if (!s_play_screen.bg[y][WIDTH - 1]) {
                s_play_screen.bg[y][WIDTH - 1] = 1;
                for (int x = 0; x < WIDTH - 1; x++)
                    s_play_screen.bg[y][x] = TB_WHITE;
                break;
            }
        }
        break;
    }
}

static void render(void) {
    tb_clear();
    switch (s_play_screen.state) {
    case STATE_PLAY:
        if (!s_play_screen.pause)
            background_render(17, 1, s_play_screen.bg);
        else
            background_render_highlighted(17, 1, s_play_screen.bg);
        tetromino_render(17, 1, &s_play_screen.current_tetromino);
        render_next_tetromino(1, 3);
        break;
    case STATE_SEMI_TETRIS:
        background_render(17, 1, s_play_screen.bg);
        render_next_tetromino(1, 3);
        break;
    case STATE_TETRIS:
        if (!s_play_screen.decay_highlight)
            background_render(17, 1, s_play_screen.bg);
        else
            background_render_highlighted(17, 1, s_play_screen.bg);
        render_next_tetromino(1, 3);
        break;
    case STATE_OVER:
        background_render(17, 1, s_play_screen.bg);
        break;
    }

    render_background_frame(16, 0);
    render_score(40, 0);
    render_lines(1, 0);
    render_tetromino_statistics(1, 7);
    if (s_play_screen.pause)
        tb_printf(24, 10, 0, 0, "PAUSED");
    tb_present();
}

static void render_score(int xoff, int yoff) {
    tb_printf(xoff, yoff + 0, 0, 0, BOX_TL BOX_HH " SCORE " BOX_H BOX_HH BOX_TR);
    tb_printf(xoff, yoff + 1, 0, 0, BOX_V "  %08d  " BOX_V, g_game.score);
    tb_printf(xoff, yoff + 2, 0, 0, BOX_BL BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_BR);
}

static void render_background_frame(int xoff, int yoff) {
    tb_printf(xoff, yoff, 0, 0, BOX_TL BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_TR);
    tb_printf(xoff, yoff + HEIGHT + 1, 0, 0,
              BOX_BL BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_BR);
    for (int y = 0; y < HEIGHT; y++) {
        tb_printf(xoff, yoff + y + 1, 0, 0, BOX_V);
        tb_printf(xoff + 21, yoff + y + 1, 0, 0, BOX_V);
    }
}

static void render_next_tetromino(int xoff, int yoff) {
    tb_printf(xoff, yoff + 0, 0, 0, BOX_TL BOX_HH BOX_H " NEXT " BOX_H BOX_HH BOX_TR);
    tb_printf(xoff, yoff + 1, 0, 0, BOX_V "            " BOX_V);
    tb_printf(xoff, yoff + 2, 0, 0, BOX_V "            " BOX_V);
    tb_printf(xoff, yoff + 3, 0, 0, BOX_BL BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_BR);

    switch (s_play_screen.next_tetromino.type) {
    case TETROMINO_TYPE_I:
    case TETROMINO_TYPE_O:
        tetromino_render(xoff + 7, yoff + 1, &s_play_screen.next_tetromino);
        break;
    default:
        tetromino_render(xoff + 6, yoff + 1, &s_play_screen.next_tetromino);
    }
}

static void render_tetromino_statistics(int xoff, int yoff) {
    for (int i = 0; i < TETROMINO_TYPE_COUNT; i++) {
        const char *utf8 = tetromino_get_1x4_utf8(i);
        uint32_t stats = s_play_screen.ttm_statistics[i];
        tb_printf(xoff, 2 * i + yoff + 1, 0, 0, BOX_V " %s %05d " BOX_V, utf8, stats);
        tb_printf(xoff, 2 * i + yoff + 2, 0, 0, BOX_V "            " BOX_V);
    }

    tb_printf(xoff, yoff + 0, 0, 0, BOX_TL " STATISTICS " BOX_TR);
    tb_printf(xoff, yoff + 14, 0, 0, BOX_BL BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_BR);
}

static void render_lines(int xoff, int yoff) {
    tb_printf(xoff, yoff + 0, 0, 0, BOX_TL BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_TR);
    tb_printf(xoff, yoff + 1, 0, 0, BOX_V " LINES %04d " BOX_V, s_play_screen.lines);
    tb_printf(xoff, yoff + 2, 0, 0, BOX_BL BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_HH BOX_BR);
}

