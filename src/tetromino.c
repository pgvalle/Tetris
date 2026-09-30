#include "tetromino.h"
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

static const char* ASCII[7] = {
    "█▀▀▀",
    "▀▀▀█",
    " ▀█▄",
    " ▄█▀",
    "▀▀▀▀",
    " ▀█▀",
    " ██ ",
};

// to avoid modulo bias
static int random_range(int min, int max) {
    float random = 1.0 * rand() / RAND_MAX;
    int range = max - min + 1;
    return min + random * range;
}

tetromino_t create_tetromino(int x, int y) {
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

const char *get_tetromino_1x4_utf8(tetromino_type_t tt) {
    return ASCII[tt];
}

void render_tetromino(int xoff, int yoff, const tetromino_t *t) {
    const point_t *pts = get_tetromino_points(t->type);
    for (int i = 0; i < 4; i++) {
        point_t pt = rotate_n_move_point(pts[i], t->deg, t->pos);
        tb_printf(2 * pt.x + xoff, pt.y + yoff, 0, t->clr, "  ");
    }
}