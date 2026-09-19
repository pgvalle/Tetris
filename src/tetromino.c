#include "tetromino.h"
#include "config.h"
#include "point.h"

#include <termbox2.h>

static point_t POINTS[][4] = {
    {{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}, // L
    {{-1, 0}, {0, 0}, {1, 0}, {1, 1}},  // J
    {{-1, 0}, {0, 0}, {0, 1}, {1, 1}},  // Z
    {{0, 0}, {1, 0}, {-1, 1}, {0, 1}},  // S
    {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}}, // I
    {{-1, 0}, {0, 0}, {1, 0}, {0, 1}},  // T
    {{0, 0}, {0, 1}, {-1, 1}, {-1, 0}}  // O
};

static tetromino_color_t COLORS[] = {TB_RED,     TB_GREEN, TB_YELLOW, TB_BLUE,
                                     TB_MAGENTA, TB_CYAN,  TB_WHITE};

// to avoid modulo bias
static int random_range(int min, int max) {
    int range = max - min + 1;
    int limit = RAND_MAX - RAND_MAX % range;
    int r;

    do {
        r = rand();
    } while (r >= limit);
    return min + (r % range);
}

tetromino_t new_tetromino(int x, int y) {
    tetromino_t t;
    t.type = random_range(0, TETROMINO_TYPE_COUNT - 1);
    t.clr = COLORS[random_range(0, 6)];
    t.pos = (point_t){x, y};
    t.deg = 0;
    return t;
}

point_t *get_tetromino_points(tetromino_type_t tt) { return POINTS[tt]; }

void rotate_tetromino(tetromino_t *t, int cw) {
    switch (t->type) {
    case TETROMINO_TYPE_O:
        break;
    case TETROMINO_TYPE_L: // all 4 orientations valid
    case TETROMINO_TYPE_J:
    case TETROMINO_TYPE_T:
        t->deg += (cw ? -1 : 1) * 90;
        break;
    case TETROMINO_TYPE_Z: // only 2 valid orientations
    case TETROMINO_TYPE_S:
    case TETROMINO_TYPE_I:
        t->deg = t->deg == 90 ? 0 : 90;
    default:
        break;
    }
}

void render_tetromino(const tetromino_t *t) {
    const point_t *pts = get_tetromino_points(t->type);
    for (int i = 0; i < 4; i++) {
        point_t pt = rotate_n_move_point(pts[i], t->deg, t->pos);
#ifdef STRETCH_X
        tb_printf(2 * pt.x, pt.y, 0, t->clr, "  ");
#else
        tb_printf(pt.x, pt.y, 0, t->clr, " ");
#endif
    }
}