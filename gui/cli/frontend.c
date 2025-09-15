#ifndef FRONTEND_C
#define FRONTEND_C

#include "frontend.h"

#include "../../brick_game/tetris/backend.h"

void init_ncurses() {
  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  timeout(100);
}

void draw_game_field(WINDOW *win, int **field) {
  box(win, ACS_VLINE, ACS_HLINE);
  mvwprintw(win, 0, 8, "TETRIS");
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (field[y][x]) {
        mvwprintw(win, y + 1, x * RATIO + 1, "[]");
      }
    }
  }
}

void draw_next_fig_frame(WINDOW *win) {
  box(win, ACS_VLINE, ACS_HLINE);
  mvwprintw(win, 0, 3, "NEXT_FIGURE");
}

void draw_game_info_frame(WINDOW *win, GameInfo_t *info) {
  box(win, ACS_VLINE, ACS_HLINE);
  mvwprintw(win, 0, 4, "GAME_INFO");
  mvwprintw(win, 1, 1, "High score");
  mvwprintw(win, 2, 1, "%d", info->high_score);
  mvwprintw(win, 4, 1, "Current score");
  mvwprintw(win, 5, 1, "%d", info->score);
  mvwprintw(win, 7, 1, "Level");
  mvwprintw(win, 8, 1, "%d", info->level);
  mvwprintw(win, 10, 1, "Speed");
  mvwprintw(win, 11, 1, "%d ms", info->speed);
  mvwprintw(win, 12, 1, "Pause: %s", is_game_paused() ? "ON" : "OFF");
}

void draw_figure(Figure *fig, WINDOW *win) {
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (fig->shape[i][j]) {
        int screen_y = fig->y + i;
        int screen_x = (fig->x + j) * RATIO;
        mvwprintw(win, screen_y + 1, screen_x + 1, "[]");
      }
    }
  }
}

void draw_next_figure(WINDOW *win, Figure *fig) {
  int start_x = 4;
  int start_y = 3;

  for (int i = 0; i < FIGURE_SIZE; ++i) {
    for (int j = 0; j < FIGURE_SIZE; ++j) {
      if (fig->shape[i][j]) {
        mvwprintw(win, start_y + i, start_x + j * 2, "[]");
      } else {
        mvwprintw(win, start_y + i, start_x + j * 2, "  ");
      }
    }
  }
}

void draw_start_screen() {
  WINDOW *start_win = newwin(19, WIDTH * RATIO + 3, 1, 1);
  box(start_win, ACS_VLINE, ACS_HLINE);

  mvwprintw(start_win, 2, 1, "= TETRIS by Coffees =");
  mvwprintw(start_win, 5, 1, "Controls:");
  mvwprintw(start_win, 6, 1, "[Left]/[Right] - Move");
  mvwprintw(start_win, 7, 1, "[Down] - Drop");
  mvwprintw(start_win, 8, 1, "'R' - Rotate");
  mvwprintw(start_win, 9, 1, "'P' - Pause");
  mvwprintw(start_win, 10, 1, "'Q' - Quit");
  mvwprintw(start_win, 14, 1, "Press SPACE to start");

  wrefresh(start_win);

  delwin(start_win);
}
// Функция для обхода непрорисовки начального экрана
void flush_input_buffer() {
  nodelay(stdscr, TRUE);
  while (getch() != ERR);
  nodelay(stdscr, FALSE);
}

int show_start_screen_and_wait() {
  timeout(-1);  // Ожидание ввода
  flush_input_buffer();
  draw_start_screen();
  int start_key = 0;
  int result = 1;
  while (1) {
    start_key = getch();
    if (start_key != ERR) {
      if (start_key == 'q' || start_key == 'Q') {
        result = 0;
        break;
      }
      if (start_key == ' ') {
        break;
      }
    }
    usleep(10000);
  }
  return result;
}

#endif