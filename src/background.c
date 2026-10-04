#include "background.h"
#include "config.h"
#include "point.h"
#include "tetromino.h"

#include <stdbool.h>
#include <string.h>
#include <termbox2.h>

void background_init(tetromino_color_t bg[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH - 1; x++) {
            bg[y][x] = BG_CLR;
        }
        bg[y][WIDTH - 1] = 0;
    }
}

void background_place_tetromino(tetromino_color_t bg[HEIGHT][WIDTH],
                                const tetromino_t *tetromino) {
    const point_t *pts = tetromino_get_points(tetromino->type);
    for (int i = 0; i < 4; i++) {
        point_t pt = point_rotate_and_translate(pts[i], tetromino->deg, tetromino->pos);
        bg[pt.y][pt.x] = tetromino->clr;
        bg[pt.y][WIDTH - 1]++;
    }
}

bool background_check_collision(const tetromino_color_t bg[HEIGHT][WIDTH],
                                const tetromino_t *tetromino) {
    const point_t *pts = tetromino_get_points(tetromino->type);

    for (int i = 0; i < 4; i++) {
        point_t pt = point_rotate_and_translate(pts[i], tetromino->deg, tetromino->pos);
        // walls collision
        if (pt.x < 0 || pt.x >= WIDTH - 1 || pt.y >= HEIGHT) {
            return true;
        }
        // bg collision
        if (pt.y >= 0 && bg[pt.y][pt.x] != BG_CLR) {
            return true;
        }
    }

    return false;
}

int background_verify_tetris(const tetromino_color_t bg[HEIGHT][WIDTH]) {
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
    }

    return semi_tetris;
}

void background_compact(tetromino_color_t bg[HEIGHT][WIDTH]) {
    tetromino_color_t aux[HEIGHT][WIDTH];
    background_init(aux);

    int y2 = HEIGHT - 1;
    for (int y1 = HEIGHT - 1; y1 >= 0; y1--) {
        if (bg[y1][WIDTH - 1] != 0) {
            memcpy(aux + y2, bg + y1, sizeof(bg[0]));
            y2--;
        }
    }

    memcpy(bg, aux, HEIGHT * sizeof(bg[0]));
}

void background_render(int xoff, int yoff,
                       const tetromino_color_t bg[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH - 1; x++) {
            tetromino_color_t c = bg[y][x];
            tb_printf(2 * x + xoff, y + yoff, 0, c, c == BG_CLR ? "  " : "░░");
        }
    }
}

void background_render_highlighted(int xoff, int yoff,
                                   const tetromino_color_t bg[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH - 1; x++) {
            tetromino_color_t c = bg[y][x];
            tb_printf(2 * x + xoff, y + yoff, 0, c, "░░");
        }
    }
}
