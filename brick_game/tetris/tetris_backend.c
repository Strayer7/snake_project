#ifndef TETRIS_BACKEND_C
#define TETRIS_BACKEND_C

#include "tetris_backend.h"
#include <string.h>

static GameInfo_t game_info = {0};
static Figure current_fig, next_fig;
static bool is_paused = false;
static bool game_over_flag = false;
static int fall_counter = 0;

// ===== MODEL =====
static bool check_collision(Figure *fig, int **field) {
  for (int i = 0; i < FIGURE_SIZE; i++)
    for (int j = 0; j < FIGURE_SIZE; j++)
      if (fig->shape[i][j]) {
        int x = fig->x + j, y = fig->y + i;
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || field[y][x])
          return true;
      }
  return false;
}

int **create_game_field(void) {
  int **field = malloc(sizeof(int *) * HEIGHT);
  for (int i = 0; i < HEIGHT; i++) field[i] = calloc(WIDTH, sizeof(int));
  return field;
}

void free_game_field(int **field) {
  if (!field) return;
  for (int i = 0; i < HEIGHT; i++) free(field[i]);
  free(field);
}

Figure create_figure(void) {
  Figure f;
  f.type = rand() % 7;
  f.rotation = 0;
  f.x = STARTING_POINT_FIG_X;
  f.y = STARTING_POINT_FIG_Y;
  for (int i = 0; i < FIGURE_SIZE; i++)
    for (int j = 0; j < FIGURE_SIZE; j++)
      f.shape[i][j] = figures[f.type][f.rotation][i][j];
  return f;
}

void rotateFigure(Figure *fig, int **field) {
  int old_rotation = fig->rotation;
  fig->rotation = (fig->rotation + 1) % 4;
  for (int i = 0; i < FIGURE_SIZE; i++)
    for (int j = 0; j < FIGURE_SIZE; j++)
      fig->shape[i][j] = figures[fig->type][fig->rotation][i][j];

  if (check_collision(fig, field)) {
    fig->rotation = old_rotation;
    for (int i = 0; i < FIGURE_SIZE; i++)
      for (int j = 0; j < FIGURE_SIZE; j++)
        fig->shape[i][j] = figures[fig->type][fig->rotation][i][j];
  }
}

void drop_figure(Figure *fig, int **field) {
  while (!check_collision(fig, field)) fig->y++;
  fig->y--;
}

void clear_lines(int **field) {
  int lines = 0;
  for (int y = HEIGHT - 1; y >= 0; y--) {
    int full = 1;
    for (int x = 0; x < WIDTH; x++)
      if (!field[y][x]) { full = 0; break; }
    if (full) {
      for (int i = y; i > 0; i--)
        memcpy(field[i], field[i-1], WIDTH * sizeof(int));
      memset(field[0], 0, WIDTH * sizeof(int));
      lines++;
      y++;
    }
  }
  switch (lines) {
    case 1: game_info.score += 100; break;
    case 2: game_info.score += 300; break;
    case 3: game_info.score += 700; break;
    case 4: game_info.score += 1500; break;
  }
}

int load_high_score(void) {
  FILE *f = fopen(HIGHSCORE_FILE, "r");
  int hs = 0;
  if (f) { fscanf(f, "%d", &hs); fclose(f); }
  return hs;
}

void save_high_score(int score) {
  FILE *f = fopen(HIGHSCORE_FILE, "w");
  if (f) { fprintf(f, "%d", score); fclose(f); }
}

void lock_and_check_hscore(Figure *current, int **field) {
  for (int i = 0; i < FIGURE_SIZE; i++)
    for (int j = 0; j < FIGURE_SIZE; j++)
      if (current->shape[i][j]) {
        int x = current->x + j, y = current->y + i;
        if (y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH)
          field[y][x] = 1;
      }

  clear_lines(field);

  int new_level = 1 + game_info.score / 600;
  if (new_level > MAX_LEVEL) new_level = MAX_LEVEL;
  if (new_level != game_info.level) {
    game_info.level = new_level;
    game_info.speed = BASE_SPEED - (new_level - 1) * SPEED_STEP;
  }

  if (game_info.score > game_info.high_score) {
    game_info.high_score = game_info.score;
    save_high_score(game_info.high_score);
  }
}

void init_game_state(void) {
  game_info.field = create_game_field();
  game_info.next = create_game_field();
  current_fig = create_figure();
  next_fig = create_figure();
  game_info.score = 0;
  game_info.high_score = load_high_score();
  game_info.level = 1;
  game_info.speed = BASE_SPEED;
  is_paused = false;
  game_over_flag = false;
  fall_counter = 0;
}

void destroy_game_state(void) {
  free_game_field(game_info.field);
  free_game_field(game_info.next);
}

GameInfo_t updateCurrentState(void) {
  static bool init = false;
  if (!init) { init_game_state(); init = true; }

  if (!game_over_flag && !is_paused) {
    fall_counter++;
    if (fall_counter >= game_info.speed / 100) {
      fall_counter = 0;
      current_fig.y++;
      if (check_collision(&current_fig, game_info.field)) {
        current_fig.y--;
        lock_and_check_hscore(&current_fig, game_info.field);
        current_fig = next_fig;
        next_fig = create_figure();
        if (check_collision(&current_fig, game_info.field))
          game_over_flag = true;
      }
    }
  }

  for (int i = 0; i < FIGURE_SIZE; i++)
    for (int j = 0; j < FIGURE_SIZE; j++)
      game_info.next[i][j] = next_fig.shape[i][j];

  game_info.pause = is_paused;
  return game_info;
}

bool is_game_paused(void) { return is_paused; }
bool is_game_over(void) { return game_over_flag; }

Figure *get_current_figure(void) { return &current_fig; }
Figure *get_next_figure(void) { return &next_fig; }

// ===== CONTROLLER =====
void userInput(UserAction_t action, bool hold) {
  (void)hold; // не используется
  if (game_over_flag) return;

  switch (action) {
    case Left:  current_fig.x--; if (check_collision(&current_fig, game_info.field)) current_fig.x++; break;
    case Right: current_fig.x++; if (check_collision(&current_fig, game_info.field)) current_fig.x--; break;
    case Down:  drop_figure(&current_fig, game_info.field); break;
    case Action: rotateFigure(&current_fig, game_info.field); break;
    case Pause: is_paused = !is_paused; break;
    default: break;
  }
}

#endif