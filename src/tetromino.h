#ifndef TETRIS_TETROMINO_H
#define TETRIS_TETROMINO_H

#include "config.h"
#include "point.h"
#include <stdbool.h>

typedef char tetromino_color_t;

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

point_t *get_tetromino_points(tetromino_type_t tt);
void rotate_tetromino(tetromino_t *t, int cw);
void render_tetromino(const tetromino_t *t);

#endif // TETRIS_TETROMINO_H