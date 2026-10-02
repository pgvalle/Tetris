#ifndef TETRIS_POINT_H
#define TETRIS_POINT_H

typedef struct {
    int x, y;
} point_t;

#define point_create(x, y) ((point_t){x, y})
point_t point_rotate_and_translate(point_t pt, int deg, point_t offset);

#endif // TETRIS_POINT_H
