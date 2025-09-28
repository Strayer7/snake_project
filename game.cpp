#include <iostream>

// ===== Snake (Qt + ncurses) =====
#include "gui/desktop/snake_frontend.h"  // Snake desktop (Qt)
#include "gui/cli/snake_console.h"       // Snake console (ncurses)

// ===== Tetris (Qt + ncurses) =====
#include "gui/desktop/tetris_frontend.h" // Tetris desktop (Qt)
extern "C" {
    #include "gui/cli/tetris_cli_frontend.h" // Tetris console (ncurses UI)
    #include "brick_game/tetris/tetris_backend.h"  // Tetris backend (логика)
}

// ==== Tetris Console ====
void run_tetris_console() {
    init_ncurses();

    if (!show_start_screen_and_wait()) {
        endwin();
        return; // если нажали Q на стартовом экране
    }

    WINDOW *game_win = newwin(HEIGHT + 2, WIDTH * RATIO + 2, 1, 1);
    WINDOW *next_win = newwin(10, 20, 1, WIDTH * RATIO + 5);
    WINDOW *info_win = newwin(15, 20, 12, WIDTH * RATIO + 5);

    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    GameInfo_t info = updateCurrentState();

    while (!is_game_over()) {
        int ch = getch();
        switch (ch) {
            case KEY_LEFT:  userInput(Left, true); break;
            case KEY_RIGHT: userInput(Right, true); break;
            case KEY_DOWN:  userInput(Down, true); break;
            case 'r': case 'R': userInput(Action, true); break;
            case 'p': case 'P': userInput(Pause, false); break;
            case 'q': case 'Q': endwin(); return;
            default: break;
        }

        info = updateCurrentState();

        werase(game_win);
        werase(next_win);
        werase(info_win);

        draw_game_field(game_win, info.field);
        draw_next_fig_frame(next_win);
        draw_next_figure(next_win, get_next_figure());
        draw_game_info_frame(info_win, &info);
        draw_figure(get_current_figure(), game_win);

        wrefresh(game_win);
        wrefresh(next_win);
        wrefresh(info_win);

        usleep(100000);
    }

    mvprintw(HEIGHT / 2, WIDTH, "GAME OVER! Press any key...");
    getch();

    delwin(game_win);
    delwin(next_win);
    delwin(info_win);
    endwin();
}

// ==== Main entry ====
int main(int argc, char *argv[]) {
    std::cout << "==== GAME PROJECT ====\n";
    std::cout << "1. Snake (Console)\n";
    std::cout << "2. Snake (Desktop)\n";
    std::cout << "3. Tetris (Console)\n";
    std::cout << "4. Tetris (Desktop)\n";
    std::cout << "Choose option: ";

    int choice;
    std::cin >> choice;

    if (choice == 1) {
        run_snake_console();
        return 0;
    }
    else if (choice == 2) {
        QApplication app(argc, argv);
        game_field w;
        w.show();
        return app.exec();
    }
    else if (choice == 3) {
        run_tetris_console();
        return 0;
    }
    else if (choice == 4) {
        QApplication app(argc, argv);
        tetris_field w;
        w.show();
        return app.exec();
    }
    else {
        std::cout << "Invalid choice.\n";
    }

    return 0;
}
