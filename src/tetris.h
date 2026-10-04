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

extern struct tetris_t g_game;

void shutdown_game(int status);

// SCREENS

void play_screen_init(void);
void play_screen_quit(void);
void play_screen_process_input(const struct tb_event *e);
void play_screen_process_frame(uint32_t delta);

#endif // TETRIS_H
