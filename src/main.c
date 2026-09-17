#define TB_IMPL
#include "../termbox2.h"
#include "tetris.h"
#include <sys/time.h>
#include <time.h>

static struct {
    enum { TETRIS, SEMI_TETRIS, PLAY } state;
    int level;
    int ticks;
    tetromino_t ttm, ttm_next;
    tetromino_color_t bg[HEIGHT][WIDTH];
    int count[HEIGHT]; // for each row, count how full is it
} g;

void spawn_next_pc();
void verify_tetris() {
    int seq = 0;
    int row_complete = 0, tetris = 0;
    for (int y = 0; y < HEIGHT; y++) {
        if (g.count[y] == WIDTH) {
            g.count[y] = -g.count[y]; // to know which rows are emptying
            seq++;
            if (seq == 4) {
                tetris = 1;
            } else {
                row_complete = 1;
            }
        } else {
            seq = 0;
        }
    }

    if (tetris) {
        g.state = TETRIS;
    } else if (row_complete) {
        g.state = SEMI_TETRIS;
    }
}

void init();
void update();
void render();

int main(int argc, char **argv) {
    init();

    int limit = 60;
    int run = 1;
    while (run) {
        struct tb_event ev;
        while (tb_peek_event(&ev, 16) != TB_ERR_NO_EVENT) {
            if (ev.type == TB_EVENT_KEY) {
                if (ev.ch == 'q')
                    run = 0;
                else if (ev.ch == 'r') {
                    rotate_tetromino(&g.ttm, 1);
                    if (collide_tetromino(&g.ttm, g.bg)) {
                        rotate_tetromino(&g.ttm, 0);
                    }
                } else if (ev.key == TB_KEY_ARROW_LEFT) {
                    g.ttm.pos.x -= 1;
                    if (collide_tetromino(&g.ttm, g.bg)) {
                        g.ttm.pos.x += 1;
                    }
                } else if (ev.key == TB_KEY_ARROW_RIGHT) {
                    g.ttm.pos.x += 1;
                    if (collide_tetromino(&g.ttm, g.bg)) {
                        g.ttm.pos.x -= 1;
                    }
                } else if (ev.key == TB_KEY_ARROW_DOWN) {
                    g.ttm.pos.y += 1;
                    if (collide_tetromino(&g.ttm, g.bg)) {
                        g.ttm.pos.y -= 1;
                        move_tetromino_to_bg(&g.ttm, g.bg);
                        g.count[g.ttm.pos.y]++; // add to the counter of that row
                        verify_tetris();
                        if (g.state == PLAY)
                            spawn_next_pc();
                    }
                }
            }
        }

        update();
        tb_clear();
        render();
        tb_present();
    }

    tb_shutdown();

    return 0;
}

void spawn_next_pc() {
    g.ttm = g.ttm_next;
    g.ttm.pos = (point_t){5, 0};

    int type = rand() % TETROMINO_TYPE_COUNT;
    g.ttm_next = (tetromino_t){type, TB_RED, {15, 5}, 0};
}

void init() {
    tb_init();
    srand(time(NULL));

    g.state = PLAY;
    g.level = 0;
    spawn_next_pc();
    spawn_next_pc();

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            g.bg[y][x] = BG_CLR;
        }
        g.count[y] = 0;
    }
}

void update() {
    switch (g.state) {
    case TETRIS:
        // play sound and some flashy extra visuals
    case SEMI_TETRIS: {
        int over = 0;
        for (int y = 0; y < HEIGHT; y++) {
            int x = -(g.count[y]++) / 2;
            if (x == 0) {
                over = 1;
            }
            g.bg[y][x] = BG_CLR;
        }
        if (over) {
            spawn_next_pc();
            g.state = PLAY;
        }
        break;}
    case PLAY:
        g.ticks++;
        if (g.ticks % 60 == 0) {
            g.ttm.pos.y += 1;
            if (collide_tetromino(&g.ttm, g.bg)) {
                g.ttm.pos.y -= 1;
                move_tetromino_to_bg(&g.ttm, g.bg);
                g.count[g.ttm.pos.y]++; // add to the counter of that row
                verify_tetris();
                if (g.state == PLAY)
                    spawn_next_pc();
                // add to scene
            }
        }
        break;
    }
}

void render() {
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
}