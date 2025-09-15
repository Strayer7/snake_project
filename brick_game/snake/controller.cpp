#include "controller.h"
#include "model.h"

static GameModel* game_model = nullptr;

GameModel& get_game_model() {
    if (game_model == nullptr) {
        game_model = new GameModel();
    }
    return *game_model;
}

void userInput(UserAction_t action, bool hold) {
    get_game_model().handle_user_input(action, hold);
}

GameInfo_t updateCurrentState() {
    GameModel& model = get_game_model();
    model.update_game_state();
    return *model.get_game_info();
}

// Функция для очистки ресурсов (может быть вызвана при завершении программы)
void cleanup_game_controller() {
    if (game_model != nullptr) {
        delete game_model;
        game_model = nullptr;
    }
}