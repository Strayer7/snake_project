#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "../../common.h"
#include "figures.h"

#define HIGHSCORE_FILE "highscore.txt"

int **create_game_field();
void free_game_field(int **field);
Figure create_figure();
void userInput(UserAction_t action, bool hold);
bool check_collision(Figure *fig, int **field);
void rotateFigure(Figure *fig, int **field);
void clear_lines(int **field);
void drop_figure(Figure *fig, int **field);
void init_figures();
Figure *get_current_figure();
Figure *get_next_figure();
void lock_and_check_hscore(Figure *current, int **field);
int load_high_score();
void save_high_score(int score);
bool is_game_paused(void);
bool is_game_over();
void destroy_game_state(void);

GameInfo_t updateCurrentState();

#endif