#ifndef BACKEND_C
#define BACKEND_C

#include "backend.h"

#include "figures.h"

#define UNUSED(x) (void)(x)
#define BASE_SPEED 700
#define SPEED_STEP 60
#define MAX_LEVEL 10

static Figure current_fig;
static Figure next_fig;
static bool is_paused = false;
static bool game_over_flag = false;
static int fall_counter = 0;
static GameInfo_t game_info = {0};

void init_game_state() {
  game_info.field = create_game_field();
  game_info.next = create_game_field();

  current_fig = create_figure();
  next_fig = create_figure();

  game_info.score = 0;
  game_info.high_score = load_high_score();
  game_info.level = 1;
  game_info.speed = BASE_SPEED;
  game_info.pause = is_paused;
}

// Главная функция обновления состояния игры. Вызывается каждый кадр игрового
// цикла. Инициализирует игру при первом вызове, управляет падением фигуры,
// обработкой столкновений, фиксацией фигур, генерацией новых фигур, обновлением
// информации для интерфейса и возвратом актуального состояния для отрисовки.
GameInfo_t updateCurrentState() {
  static bool is_initialized = false;  // для соханения значения между вызовами

  if (!is_initialized) {
    init_game_state();
    is_initialized = true;
  } else {
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

          // Если новая фигура сразу не помещается — gg
          if (check_collision(&current_fig, game_info.field)) {
            game_over_flag = true;
          }
        }
      }
    }

    // Тут копируем данные фигуры next, которую созадли выше для отображения в
    // консоли
    for (int i = 0; i < FIGURE_SIZE; ++i)
      for (int j = 0; j < FIGURE_SIZE; ++j)
        game_info.next[i][j] = next_fig.shape[i][j];

    game_info.pause = is_paused;
  }

  return game_info;
}

void userInput(UserAction_t action, bool hold) {
  UNUSED(hold);  // пока не используется

  switch (action) {
    case Left:
      current_fig.x--;
      if (check_collision(&current_fig, game_info.field)) current_fig.x++;
      break;
    case Right:
      current_fig.x++;
      if (check_collision(&current_fig, game_info.field)) current_fig.x--;
      break;
    case Down:
      drop_figure(&current_fig, game_info.field);
      break;
    case Action:
      rotateFigure(&current_fig, game_info.field);
      break;
    case Pause:
      is_paused = !is_paused;
      break;
    default:
      break;
  }
}

bool is_game_paused() { return is_paused; }

bool is_game_over() { return game_over_flag; }

void drop_figure(Figure *fig, int **field) {
  while (!check_collision(fig, field)) {
    fig->y++;
  }
  fig->y--;
}

void rotateFigure(Figure *fig, int **field) {
  int old_rotation = fig->rotation;
  fig->rotation = (fig->rotation + 1) % 4;

  int old_x = fig->x;
  int old_y = fig->y;

  for (int i = 0; i < FIGURE_SIZE; ++i)
    for (int j = 0; j < FIGURE_SIZE; ++j)
      fig->shape[i][j] = figures[fig->type][fig->rotation][i][j];

  if (check_collision(fig, field)) {
    fig->x -= 1;  // двигаем влево
    if (!check_collision(fig, field)) {
    } else {
      fig->x += 2;  // двигаем вправо
      if (!check_collision(fig, field)) {
      } else {
        // reset если сталкиваемся
        fig->x = old_x;
        fig->y = old_y;
        fig->rotation = old_rotation;
        for (int i = 0; i < FIGURE_SIZE; ++i)
          for (int j = 0; j < FIGURE_SIZE; ++j)
            fig->shape[i][j] = figures[fig->type][old_rotation][i][j];
      }
    }
  }
}

bool check_collision(Figure *fig, int **field) {
  bool result = false;
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (fig->shape[i][j]) {
        int x = fig->x + j;
        int y = fig->y + i;
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT ||
            field[y][x]) {  // Проверка границ поля и блоков
          result = true;
        }
      }
    }
  }
  return result;
}

Figure create_figure() {
  Figure new_fig;
  new_fig.type = rand() % 7;
  new_fig.rotation = 0;
  new_fig.x = STARTING_POINT_FIG_X;
  new_fig.y = STARTING_POINT_FIG_Y;

  for (int i = 0; i < FIGURE_SIZE; ++i)
    for (int j = 0; j < FIGURE_SIZE; ++j)
      new_fig.shape[i][j] = figures[new_fig.type][new_fig.rotation][i][j];

  return new_fig;
}

int **create_game_field() {
  int **field = malloc(sizeof(int *) * HEIGHT);
  for (int i = 0; i < HEIGHT; i++) field[i] = calloc(WIDTH, sizeof(int));
  return field;
}

void free_game_field(int **field) {
  for (int i = 0; i < HEIGHT; ++i) free(field[i]);
  free(field);
}

Figure *get_current_figure() { return &current_fig; }
Figure *get_next_figure() { return &next_fig; }

void init_figures() {
  current_fig = create_figure();
  next_fig = create_figure();
}

void lock_and_check_hscore(Figure *current, int **field) {
  for (int i = 0; i < FIGURE_SIZE; ++i) {
    for (int j = 0; j < FIGURE_SIZE; ++j) {
      if (current->shape[i][j]) {
        int x = current->x + j;
        int y = current->y + i;
        if (y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH) field[y][x] = 1;
      }
    }
  }

  clear_lines(field);

  // Тут механика уровней и скорости и сейв хайскора
  int new_level =
      1 +
      game_info.score /
          600;  // 1+ потому что всегда начинается с 1 уровня, вычисляем уровень
  if (new_level > MAX_LEVEL) new_level = MAX_LEVEL;
  if (new_level != game_info.level) {
    game_info.level = new_level;
    game_info.speed = BASE_SPEED - (game_info.level - 1) * SPEED_STEP;
  }

  if (game_info.score > game_info.high_score) {
    game_info.high_score = game_info.score;
    save_high_score(game_info.high_score);
  }
}

void clear_lines(int **field) {
  int lines_cleared = 0;

  for (int y = HEIGHT - 1; y >= 0; y--) {
    int full_line = 1;
    for (int x = 0; x < WIDTH; x++) {
      if (!field[y][x]) {
        full_line = 0;
        break;
      }
    }

    if (full_line) {
      for (int i = y; i > 0; i--)
        memcpy(field[i], field[i - 1], WIDTH * sizeof(int));  // Копируем строки

      memset(field[0], 0, WIDTH * sizeof(int));
      lines_cleared++;
      y++;  // повторяем проверку для этой же строки
    }
  }

  switch (lines_cleared) {
    case 1:
      game_info.score += 100;
      break;
    case 2:
      game_info.score += 300;
      break;
    case 3:
      game_info.score += 700;
      break;
    case 4:
      game_info.score += 1500;
      break;
  }
}

int load_high_score() {
  FILE *f = fopen(HIGHSCORE_FILE, "r");
  int hs = 0;
  if (f) {
    fscanf(f, "%d", &hs);
    fclose(f);
  }
  return hs;
}

void save_high_score(int score) {
  FILE *f = fopen(HIGHSCORE_FILE, "w");
  if (f) {
    fprintf(f, "%d", score);
    fclose(f);
  }
}

void destroy_game_state() {
  if (game_info.field) {
    free_game_field(game_info.field);
    game_info.field = NULL;
  }
  if (game_info.next) {
    free_game_field(game_info.next);
    game_info.next = NULL;
  }
}

#endif