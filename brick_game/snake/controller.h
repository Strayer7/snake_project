// game_controller.h
#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "../common.h"

class GameModel;
GameModel& get_game_model();

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();
void cleanup_game_controller();

#endif //GAME_CONTROLLER_H