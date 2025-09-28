#include <ncurses.h>
#include <unistd.h>     // usleep
#include "../../brick_game/snake/controller.h"
#include "../../brick_game/snake/model.h"

void draw_game(const GameInfo_t& info) {
    // размеры окон
    int field_width  = WIDTH * 2 + 2;   // ×2 для символов + рамка
    int field_height = HEIGHT + 2;      // + рамка
    int panel_width  = 20;
    int panel_height = field_height;

    // создаём окна
    WINDOW* field_win = newwin(field_height, field_width, 0, 0);
    WINDOW* info_win  = newwin(panel_height, panel_width, 0, field_width + 1);

    // === 1. Игровое поле ===
    werase(field_win);
    box(field_win, 0, 0);
    mvwprintw(field_win, 0, field_width / 2 - 3, "SNAKE");

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int value = info.field[y][x];
            if (value == 1) {
                mvwprintw(field_win, y + 1, x * 2 + 1, "oo"); // тело
            } else if (value == 2) {
                mvwprintw(field_win, y + 1, x * 2 + 1, "OO"); // голова
            } else if (value == 3) {
                mvwprintw(field_win, y + 1, x * 2 + 1, "[]"); // яблоко
            }
        }
    }

    wrefresh(field_win);

    // === 2. Панель с информацией ===
    werase(info_win);
    box(info_win, 0, 0);
    mvwprintw(info_win, 0, 4, "GAME INFO");
    mvwprintw(info_win, 2, 1, "High score:");
    mvwprintw(info_win, 3, 1, "%d", info.high_score);
    mvwprintw(info_win, 5, 1, "Score:");
    mvwprintw(info_win, 6, 1, "%d", info.score);
    mvwprintw(info_win, 8, 1, "Level:");
    mvwprintw(info_win, 9, 1, "%d", info.level);
    mvwprintw(info_win, 11, 1, "Speed:");
    mvwprintw(info_win, 12, 1, "%d ms", info.speed);
    mvwprintw(info_win, 14, 1, "Pause: %s", info.pause ? "ON" : "OFF");

    wrefresh(info_win);

    delwin(field_win);
    delwin(info_win);
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
        case ' ':           userInput(Action, true); break;  // Boost press
        default: break;
    }

    // проверяем отпускание пробела
    if (!(ch == ' ')) {
        // если пробел не зажат — сброс
        userInput(Action, false);
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

        // --- Задержка с учётом буста ---
        int current_speed = info.speed;
        if (get_game_model().is_speed_boost_active()) {
            current_speed = std::max(50, info.speed / 2); // или info.speed / 1.5
        }
        usleep(current_speed * 1000); // только одна задержка
    }

    endwin(); // выход из ncurses
}