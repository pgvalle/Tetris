#include "tetris.h"
#include "../termbox2.h"

void move_tetromino_to_bg(const tetromino_t *t,
                          tetromino_color_t bg[HEIGHT][WIDTH]) {
    const point_t *pts = get_tetromino_points(t->type);
    for (int i = 0; i < 4; i++) {
        point_t pt = rotate_n_move_point(pts[i], t->deg, t->pos);
        bg[pt.y][pt.x] = t->clr;
    }
}

bool collide_tetromino(tetromino_t *t, tetromino_color_t bg[HEIGHT][WIDTH]) {
    const point_t *pts = get_tetromino_points(t->type);

    for (int i = 0; i < 4; i++) {
        point_t pt = rotate_n_move_point(pts[i], t->deg, t->pos);
        // walls collision
        if (pt.x < 0 || pt.x >= WIDTH || pt.y >= HEIGHT)
            return true;
        // bg collision
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (bg[y][x] != BG_CLR && pt.x == x && pt.y == y)
                    return true;
            }
        }
    }

    return false;
}

void render_bg(tetromino_color_t bg[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            tetromino_color_t clr = bg[y][x];
#ifdef STRETCH_X
            tb_printf(2 * x, y, 0, clr, clr == BG_CLR ? "  " : "x ");
#else
            tb_printf(x, y, 0, clr, clr == BG_CLR ? " " : "x");
#endif
        }
    }
}