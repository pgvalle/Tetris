#ifndef TETRIS_TETROMINO_H
#define TETRIS_TETROMINO_H

#include "point.h"

#include <stdbool.h>
#include <termbox2.h>

typedef uintattr_t tetromino_color_t;

typedef enum {
    TETROMINO_TYPE_L = 0,
    TETROMINO_TYPE_J,
    TETROMINO_TYPE_Z,
    TETROMINO_TYPE_S,
    TETROMINO_TYPE_I,
    TETROMINO_TYPE_T,
    TETROMINO_TYPE_O,
    TETROMINO_TYPE_COUNT
} tetromino_type_t;

typedef struct {
    tetromino_type_t type;
    tetromino_color_t clr;
    point_t pos;
    int deg;
} tetromino_t;

tetromino_t tetromino_create(int x, int y);
const point_t *tetromino_get_points(tetromino_type_t tt);
const char *tetromino_get_1x4_utf8(tetromino_type_t tt);
void tetromino_rotate(tetromino_t *tetromino, bool clockwise);
void tetromino_render(int xoff, int yoff, const tetromino_t *tetromino);

#endif // TETRIS_TETROMINO_H
