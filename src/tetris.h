#ifndef TETRIS_H
#define TETRIS_H

#include "config.h"
#include "point.h"
#include "tetromino.h"

void move_tetromino_to_bg(const tetromino_t *t, tetromino_color_t bg[HEIGHT][WIDTH]);
bool collide_tetromino(tetromino_t *t, const tetromino_color_t bg[HEIGHT][WIDTH]);
void render_bg(const tetromino_color_t bg[HEIGHT][WIDTH]);

#endif 