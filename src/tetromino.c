#include "tetromino.h"
#include "point.h"

#include <stdlib.h>
#include <string.h>
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

static tetromino_color_t COLORS[] = {TB_RED, TB_GREEN, TB_BLUE, TB_MAGENTA};

static const char *ASCII[7] = {
    "█▀▀▀", "▀▀▀█", " ▀█▄", " ▄█▀", "▀▀▀▀", " ▀█▀", " ██ ",
};

static int random_range(int low, int high) {
    return low + rand() % (high - low + 1);
}

static struct {
    tetromino_color_t colors[4];
    tetromino_type_t types[TETROMINO_TYPE_COUNT];
    int len_colors;
    int len_types;
} bags = {.len_colors = 0, .len_types = 0};

static tetromino_color_t next_rand_color(void) {
    if (bags.len_colors == 0) {
        for (int i = 0; i < 4; i++) {
            bags.colors[i] = COLORS[i];
        }
        bags.len_colors = 4;
    }

    int r = random_range(0, bags.len_colors - 1);
    tetromino_color_t clr = bags.colors[r];
    memmove(bags.colors + r, bags.colors + r + 1,
            (bags.len_colors - r - 1) * sizeof(bags.colors[0]));
    bags.len_colors--;
    return clr;
}

static tetromino_type_t next_rand_type(void) {
    if (bags.len_types == 0) {
        for (int i = 0; i < TETROMINO_TYPE_COUNT; i++) {
            bags.types[i] = i;
        }
        bags.len_types = TETROMINO_TYPE_COUNT;
    }

    int r = random_range(0, bags.len_types - 1);
    tetromino_type_t tt = bags.types[r];
    memmove(bags.types + r, bags.types + r + 1,
            (bags.len_types - r - 1) * sizeof(bags.types[0]));
    bags.len_types--;
    return tt;
}

tetromino_t tetromino_create(int x, int y) {
    tetromino_t tetromino;
    tetromino.type = next_rand_type();
    tetromino.clr = next_rand_color();
    tetromino.pos = point_create(x, y);
    tetromino.deg = 0;
    return tetromino;
}

const point_t *tetromino_get_points(tetromino_type_t tt) {
    return POINTS[tt];
}

void tetromino_rotate(tetromino_t *tetromino, bool clockwise) {
    switch (tetromino->type) {
    case TETROMINO_TYPE_O:
        break;
    case TETROMINO_TYPE_L: // all 4 orientations valid
    case TETROMINO_TYPE_J:
    case TETROMINO_TYPE_T:
        tetromino->deg += (clockwise ? -1 : 1) * 90;
        break;
    case TETROMINO_TYPE_Z: // only 2 valid orientations
    case TETROMINO_TYPE_S:
    case TETROMINO_TYPE_I:
        tetromino->deg = tetromino->deg == 90 ? 0 : 90;
        break;
    default:
        break;
    }
}

const char *tetromino_get_1x4_utf8(tetromino_type_t tt) {
    return ASCII[tt];
}

void tetromino_render(int xoff, int yoff, const tetromino_t *tetromino) {
    const point_t *pts = tetromino_get_points(tetromino->type);
    for (int i = 0; i < 4; i++) {
        point_t pt = point_rotate_and_translate(pts[i], tetromino->deg, tetromino->pos);
        tb_printf(2 * pt.x + xoff, pt.y + yoff, 0, tetromino->clr, "  ");
    }
}


