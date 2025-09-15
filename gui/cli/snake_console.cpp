#include <ncurses.h>
#include <unistd.h>     // usleep
#include "back/controller.h"
#include "back/model.h"

void draw_game(const GameInfo_t& info) {
    clear(); // очищаем экран ncurses

    // Игровое поле
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int value = info.field[y][x];
            if (value == 0) {
                mvprintw(y, x * 2, "  ");      // пусто
            } else if (value == 1) {
                mvprintw(y, x * 2, "oo");      // тело
            } else if (value == 2) {
                mvprintw(y, x * 2, "OO");      // голова
            } else if (value == 3) {
                mvprintw(y, x * 2, "[]");      // яблоко
            }
        }
    }

    // Панель с информацией
    int info_y = HEIGHT + 1;
    mvprintw(info_y, 0, "Score: %d   High: %d", info.score, info.high_score);
    mvprintw(info_y + 1, 0, "Level: %d   Speed: %dms", info.level, info.speed);
    mvprintw(info_y + 2, 0, "Controls: WASD / Arrows, P=Pause, Enter=Start, Esc=Quit, Space=Boost");

    refresh(); // обновляем экран
}

void handle_input() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
        case 'w': case 'W': userInput(Up, true); break;
        case KEY_DOWN:
        case 's': case 'S': userInput(Down, true); break;
        case KEY_LEFT:
        case 'a': case 'A': userInput(Left, true); break;
        case KEY_RIGHT:
        case 'd': case 'D': userInput(Right, true); break;
        case 'p': case 'P': userInput(Pause, false); break;
        case 10:            userInput(Start, false); break;   // Enter
        case 27:            userInput(Terminate, false); break; // Esc
        case ' ':           userInput(Action, true); break;  // Boost
        default: break;
    }
}

void run_snake_console() {
    // Инициализация ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); // getch не блокирует

    userInput(Start, false);  // запускаем игру

    while (true) {
        handle_input();

        GameInfo_t info = updateCurrentState();
        draw_game(info);

        if (get_game_model().get_state() == STATE_GAME_OVER) {
            mvprintw(HEIGHT / 2, WIDTH, "GAME OVER! Press ENTER to restart or ESC to quit.");
            refresh();

            while (true) {
                int ch = getch();
                if (ch == 10) { userInput(Start, false); break; } // restart
                if (ch == 27) { endwin(); return; }               // quit
                usleep(10000);
            }
        }

        usleep(info.speed * 1000); // задержка в мс → мкс
    }

    endwin(); // выход из ncurses
}
