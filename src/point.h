#ifndef TETRIS_POINT_H
#define TETRIS_POINT_H

typedef struct {
    int x, y;
} point_t;

point_t rotate_n_move_point(point_t pt, int deg, point_t off);

#endif // TETRIS_POINT_H