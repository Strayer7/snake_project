#ifndef TETRIS_BACKEND_H
#define TETRIS_BACKEND_H

#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "../../common.h"
#include "figures.h"

#define HIGHSCORE_FILE "highscore.txt"
#define BASE_SPEED 700
#define SPEED_STEP 60
#define MAX_LEVEL 10

// ===== Model API =====
void init_game_state(void);
void destroy_game_state(void);
GameInfo_t updateCurrentState(void);
bool is_game_paused(void);
bool is_game_over(void);

// ===== Controller API =====
void userInput(UserAction_t action, bool hold);

// ===== Internal Model Helpers (используются внутри backenda) =====
int **create_game_field(void);
void free_game_field(int **field);
Figure create_figure(void);
void rotateFigure(Figure *fig, int **field);
void drop_figure(Figure *fig, int **field);
void lock_and_check_hscore(Figure *current, int **field);
void clear_lines(int **field);
int load_high_score(void);
void save_high_score(int score);
Figure *get_current_figure(void);
Figure *get_next_figure(void);

#endif
