#include "bg.h"
#include "config.h"
#include "point.h"
#include "tetromino.h"

#include <stdbool.h>
#include <termbox2.h>

void init_bg(tetromino_color_t bg[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH - 1; x++) {
            bg[y][x] = BG_CLR;
        }
        bg[y][WIDTH - 1] = 0;
    }  
}

void move_tetromino_to_bg(tetromino_color_t bg[HEIGHT][WIDTH],
                          const tetromino_t *t) {
    const point_t *pts = get_tetromino_points(t->type);
    for (int i = 0; i < 4; i++) {
        point_t pt = rotate_n_move_point(pts[i], t->deg, t->pos);
        bg[pt.y][pt.x] = t->clr;
        bg[pt.y][WIDTH - 1]++;
    }
}

bool collide_tetromino(const tetromino_color_t bg[HEIGHT][WIDTH],
                       tetromino_t *t) {
    const point_t *pts = get_tetromino_points(t->type);

    for (int i = 0; i < 4; i++) {
        point_t pt = rotate_n_move_point(pts[i], t->deg, t->pos);
        // walls collision
        if (pt.x < 0 || pt.x >= WIDTH - 1 || pt.y >= HEIGHT)
            return true;
        // bg collision
        if (bg[pt.y][pt.x] != BG_CLR)
            return true;
    }

    return false;
}

int verify_tetris(const tetromino_color_t bg[HEIGHT][WIDTH]) {
    int seq = 0;
    int semi_tetris = 0;
    for (int y = 0; y < HEIGHT; y++) {
        if (bg[y][WIDTH - 1] == WIDTH - 1) {
            semi_tetris = 1;
            seq++;
        } else {
            seq = 0;
        }

        if (seq == 4) {
            return 2; // full tetris
        }
    };

    return semi_tetris;
}

void compact_bg(tetromino_color_t bg[HEIGHT][WIDTH]) {
    tetromino_color_t aux[HEIGHT][WIDTH];
    init_bg(aux);
   
    int y2 = HEIGHT - 1;
    for (int y1 = HEIGHT - 1; y1 >= 0; y1--) {
        if (bg[y1][WIDTH - 1] != 0) {
            memcpy(aux + y2, bg + y1, sizeof(bg[0]));
            y2--;
        }
    }

    memcpy(bg, aux, HEIGHT * sizeof(bg[0]));
}

void render_bg(int xoff, int yoff, const tetromino_color_t bg[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH - 1; x++) {
            tetromino_color_t clr = bg[y][x];
#ifdef STRETCH_X
            tb_printf(2 * (x + xoff), y + yoff, 0, clr, clr == BG_CLR ? "  " : "░░");
#else
            tb_printf(x + xoff, y + yoff, 0, clr, clr == BG_CLR ? " " : "░");
#endif
        }
    }
}