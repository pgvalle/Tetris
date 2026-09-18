#ifndef TETRIS_BG_H
#define TETRIS_BG_H

#include "config.h"
#include "tetromino.h"

#include <stdbool.h>

void move_tetromino_to_bg(tetromino_color_t bg[HEIGHT][WIDTH],
                          const tetromino_t *t);
bool collide_tetromino(const tetromino_color_t bg[HEIGHT][WIDTH],
                       tetromino_t *t);
void render_bg(const tetromino_color_t bg[HEIGHT][WIDTH]);

#endif // TETRIS_BG_H