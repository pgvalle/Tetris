#ifndef TETRIS_BG_H
#define TETRIS_BG_H

#include "config.h"
#include "tetromino.h"

#include <stdbool.h>

void background_init(tetromino_color_t bg[HEIGHT][WIDTH]);
void background_place_tetromino(tetromino_color_t bg[HEIGHT][WIDTH],
                                const tetromino_t *tetromino);
bool background_check_collision(const tetromino_color_t bg[HEIGHT][WIDTH],
                                const tetromino_t *tetromino);
// 2 -> tetris, 1 -> rows completed, 0 -> nothing
int background_verify_tetris(const tetromino_color_t bg[HEIGHT][WIDTH]);
void background_compact(tetromino_color_t bg[HEIGHT][WIDTH]);
void background_render(int xoff, int yoff,
                       const tetromino_color_t bg[HEIGHT][WIDTH]);
void background_render_highlighted(int xoff, int yoff,
                                   const tetromino_color_t bg[HEIGHT][WIDTH]);

#endif // TETRIS_BG_H
