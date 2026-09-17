#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define WIDTH 10
#define HEIGHT 20

typedef char color_t;

typedef struct {
    int x, y;
} point_t;

typedef enum tetromino_type_t {
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
    color_t clr;
    point_t pos;
    int deg;
} tetromino_t;

point_t *get_tetromino_points(tetromino_type_t tt);
point_t rotate_n_move_point(point_t pt, int deg, point_t off);
void rotate_tetromino(tetromino_t *t, int cw);
bool collide(tetromino_t *t, color_t bg[HEIGHT][WIDTH], color_t bg_clr);
void render_tetromino(const tetromino_t *t);
void render_bg(color_t bg[HEIGHT][WIDTH], color_t bg_clr);

#endif 