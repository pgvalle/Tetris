#include "../termbox2.h"
#include "tetris.h"
#include <math.h>

#define PI 3.14159265358979323846

static point_t tt_pts[][4] = {
    {{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}, // L
    {{-1, 0}, {0, 0}, {1, 0}, {1, 1}},  // J
    {{-1, 0}, {0, 0}, {0, 1}, {1, 1}},  // Z
    {{0, 0}, {1, 0}, {-1, 1}, {0, 1}},  // S
    {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}}, // I
    {{-1, 0}, {0, 0}, {1, 0}, {0, 1}},  // T
    {{0, 0}, {0, 1}, {-1, 1}, {-1, 0}}  // O
};

point_t *get_tetromino_points(tetromino_type_t tt) {
    return tt_pts[tt];
}

point_t rotate_n_move_point(point_t pt, int deg, point_t off) {
    double rad = (PI * deg) / 180.0;
    point_t out;
    out.x = round(pt.x * cos(rad) - pt.y * sin(rad)) + off.x;
    out.y = round(pt.x * sin(rad) + pt.y * cos(rad)) + off.y;
    return out;
}

void rotate_tetromino(tetromino_t *t, int cw) {
    if (t->type == TETROMINO_TYPE_O) return;
    t->deg += (cw ? -1 : 1) * 90;
}

bool collide(tetromino_t *t, color_t bg[HEIGHT][WIDTH], color_t bg_clr) {
    const point_t *pts = get_tetromino_points(t->type);

    for (int i = 0; i < 4; i++) {
        point_t pt = rotate_n_move_point(pts[i], t->deg, t->pos);
        // walls collision
        if (pt.x < 0 || pt.x >= WIDTH || pt.y >= HEIGHT) {
            return 1;
        }
        // bg collision
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (bg[y][x] != bg_clr && pt.x == x && pt.y == y) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

void render_tetromino(const tetromino_t *t) {
    const point_t *pts = get_tetromino_points(t->type);
    for (int i = 0; i < 4; i++) {
        point_t pt = rotate_n_move_point(pts[i], t->deg, t->pos);
        tb_printf(2 * pt.x, pt.y, 0, t->clr, "  ");
    }
}

void render_bg(color_t bg[HEIGHT][WIDTH], color_t bg_clr) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            color_t clr = bg[y][x];
            tb_printf(2 * x, y, 0, clr, clr == bg_clr ? "  " : "x ");
        }
    }
}