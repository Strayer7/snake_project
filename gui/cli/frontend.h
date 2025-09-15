#ifndef FRONTEND_H
#define FRONTEND_H

#include <ncurses.h>
#include <unistd.h>

#include "../../common.h"

void init_ncurses();
void draw_game_field(WINDOW *win, int **field);
void draw_next_fig_frame(WINDOW *win);
void draw_game_info_frame(WINDOW *win, GameInfo_t *info);
void draw_figure(Figure *fig, WINDOW *win);
void draw_next_figure(WINDOW *win, Figure *fig);
void draw_start_screen(void);
void flush_input_buffer();
int show_start_screen_and_wait();

#endif