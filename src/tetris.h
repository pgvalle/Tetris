#ifndef TETRIS_H
#define TETRIS_H

#include "config.h"
#include "tetromino.h"

#include <termbox2.h>
#include <miniaudio.h>

#include <stdint.h>

enum tetris_screen_t { SCREEN_SPLASH, SCREEN_MENU, SCREEN_PLAY, SCREEN_OVER };

struct tetris_t {
    enum tetris_screen_t screen;
    int score;
    int level;
    ma_engine ma_eng;
    bool init;
};

extern struct tetris_t t;

void shutdown(int status);

// SCREENS

void init_play_screen();
void quit_play_screen();
void process_play_screen_input_event(const struct tb_event *e);
void process_play_screen_frame_event(uint32_t delta);

#endif // TETRIS_GLOBAL_H