#ifndef TETRIS_BG_H
#define TETRIS_BG_H

#include "config.h"
#include "tetromino.h"

#include <stdbool.h>

void init_background(tetromino_color_t bg[HEIGHT][WIDTH]);
void move_tetromino_to_background(tetromino_color_t bg[HEIGHT][WIDTH],
                                  const tetromino_t *t);
bool collide_tetromino(const tetromino_color_t bg[HEIGHT][WIDTH],
                       tetromino_t *t);
// 2 -> tetris, 1 -> rows completed, 0 -> nothing
int verify_tetris(const tetromino_color_t bg[HEIGHT][WIDTH]);
void compact_background(tetromino_color_t bg[HEIGHT][WIDTH]);
void render_background(int xoff, int yoff,
                       const tetromino_color_t bg[HEIGHT][WIDTH]);

#endif // TETRIS_BG_H