#include "model.h"
#include <QDebug>

SnakeModel::SnakeModel() {
    body.push_back(Coords{SNAKE_START_POINT_X, SNAKE_START_POINT_Y});
    body.push_back(Coords{SNAKE_START_POINT_X, SNAKE_START_POINT_Y+1});
    body.push_back(Coords{SNAKE_START_POINT_X, SNAKE_START_POINT_Y+2});
    body.push_back(Coords{SNAKE_START_POINT_X, SNAKE_START_POINT_Y+3});
    snake_direction = Direction::Up;
}

const std::vector<Coords>& SnakeModel::get_body() const {
    return body;
}

int SnakeModel::get_size() const {
    return static_cast<int>(body.size());
}

Direction SnakeModel::get_direction() const {
    return snake_direction;
}

void SnakeModel::set_direction(Direction new_direction) {
    switch (snake_direction) {
        case Direction::Up:
            if (new_direction != Direction::Down) snake_direction = new_direction;
            break;
        case Direction::Down:
            if (new_direction != Direction::Up) snake_direction = new_direction;
            break;
        case Direction::Left:
            if (new_direction != Direction::Right) snake_direction = new_direction;
            break;
        case Direction::Right:
            if (new_direction != Direction::Left) snake_direction = new_direction;
            break;
    }
}

bool SnakeModel::move(const Coords& apple_pos) {
    Coords new_head = body[0];
    Direction dir = get_direction();

    switch (dir) {
        case Direction::Up:    new_head.y--; break;
        case Direction::Down:  new_head.y++; break;
        case Direction::Left:  new_head.x--; break;
        case Direction::Right: new_head.x++; break;
    }

    body.insert(body.begin(), new_head);

    bool ate = (new_head.x == apple_pos.x && new_head.y == apple_pos.y);

    if (!ate) {
        body.pop_back();
    }

    return ate;
}

void SnakeModel::reset() {
    body.clear();
    body.push_back(Coords{SNAKE_START_POINT_X, SNAKE_START_POINT_Y});
    body.push_back(Coords{SNAKE_START_POINT_X, SNAKE_START_POINT_Y+1});
    body.push_back(Coords{SNAKE_START_POINT_X, SNAKE_START_POINT_Y+2});
    body.push_back(Coords{SNAKE_START_POINT_X, SNAKE_START_POINT_Y+3});
    snake_direction = Direction::Up;
}

// GameModel implementation
GameModel::GameModel() : state(STATE_START), is_speed_boost_active_(false) {
    game_info.field = nullptr;
    game_info.next = nullptr;
    game_info.score = 0;
    game_info.high_score = load_high_score();
    game_info.level = 1;
    game_info.speed = 100;
    game_info.pause = 0;

    // 🔽 Раскомментируй следующую строку, если хочешь создать поле сразу:
    // initialize_game();
}

GameModel::~GameModel() {
    free_field();
}

void GameModel::initialize_game() {
    free_field();
    create_field();
    set_apple_position();
    update_game_field();
}

GameInfo_t* GameModel::get_game_info() {
    return &game_info;
}

const GameInfo_t* GameModel::get_game_info() const {
    return &game_info;
}

void GameModel::create_field() {
    game_info.field = new int*[HEIGHT];
    for (int i = 0; i < HEIGHT; i++) {
        game_info.field[i] = new int[WIDTH]();  // () — обнуляет
    }
}

void GameModel::free_field() {
    if (game_info.field != nullptr) {
        for (int i = 0; i < HEIGHT; i++) {
            delete[] game_info.field[i];
        }
        delete[] game_info.field;
        game_info.field = nullptr;
    }
}

void GameModel::clear_game_field() {
    if (game_info.field != nullptr) {
        for (int row = 0; row < HEIGHT; ++row) {
            for (int col = 0; col < WIDTH; ++col) {
                game_info.field[row][col] = 0;
            }
        }
    }
}

void GameModel::update_game_field() {
    if (game_info.field == nullptr) return;
    clear_game_field();
    update_snake_on_field();
    draw_apple_on_field();
}

void GameModel::update_snake_on_field() {
    if (game_info.field == nullptr) return;

    const std::vector<Coords>& snake_body = snake.get_body();
    for (size_t i = 0; i < snake_body.size(); i++) {
        Coords snake_coords = snake_body[i];
        if (snake_coords.x >= 0 && snake_coords.x < WIDTH &&
            snake_coords.y >= 0 && snake_coords.y < HEIGHT) {
            if (i == 0) {
                game_info.field[snake_coords.y][snake_coords.x] = 2;
            } else {
                game_info.field[snake_coords.y][snake_coords.x] = 1;
            }
        }
    }
}

void GameModel::draw_apple_on_field() {
    if (game_info.field == nullptr) return;

    if (apple_position.x >= 0 && apple_position.x < WIDTH &&
        apple_position.y >= 0 && apple_position.y < HEIGHT) {
        game_info.field[apple_position.y][apple_position.x] = 3;
    }
}

void GameModel::handle_user_input(UserAction_t action, bool hold) {
    Q_UNUSED(hold)

    switch (action) {
        case Start:
            if (state == STATE_START || state == STATE_GAME_OVER) {
                state = STATE_PLAY;
                game_info.score = 0;           // ← ТОЛЬКО score сбрасывается
                game_info.level = 1;
                game_info.speed = 250;
                game_info.pause = 0;
                snake.reset();
                initialize_game();
            }
            break;
        case Pause:
            if (state == STATE_PLAY) {
                game_info.pause = !game_info.pause;
            }
            break;
        case Terminate:
            state = STATE_GAME_OVER;
            break;
        case Up:
            if (state == STATE_PLAY && !game_info.pause) {
                snake.set_direction(Direction::Up);
            }
            break;
        case Down:
            if (state == STATE_PLAY && !game_info.pause) {
                snake.set_direction(Direction::Down);
            }
            break;
        case Left:
            if (state == STATE_PLAY && !game_info.pause) {
                snake.set_direction(Direction::Left);
            }
            break;
        case Right:
            if (state == STATE_PLAY && !game_info.pause) {
                snake.set_direction(Direction::Right);
            }
            break;
        case Action:
            if (state == STATE_PLAY && !game_info.pause) {
                is_speed_boost_active_ = hold;  // true при удержании, false при отпускании
            }
            break;
    }
}

void GameModel::update_game_state() {
    if (game_info.field == nullptr) return;

    if (state == STATE_PLAY && !game_info.pause) {
        Coords new_head = snake.get_body()[0];
        Direction dir = snake.get_direction();

        switch (dir) {
            case Direction::Up:    new_head.y--; break;
            case Direction::Down:  new_head.y++; break;
            case Direction::Left:  new_head.x--; break;
            case Direction::Right: new_head.x++; break;
        }

        // Столкновение со стеной
        if (new_head.x < 0 || new_head.x >= WIDTH || new_head.y < 0 || new_head.y >= HEIGHT) {
            state = STATE_GAME_OVER;
            return;
        }

        // Само на себя
        const auto& body = snake.get_body();
        for (size_t i = 1; i < body.size(); ++i) {  // начиная с 1, т.к. голова = new_head
            if (new_head.x == body[i].x && new_head.y == body[i].y) {
                state = STATE_GAME_OVER;
                return;
            }
        }

        bool ate = snake.move(apple_position);

        if (ate) {
            game_info.score += 10;

            if (game_info.score > game_info.high_score) {
                game_info.high_score = game_info.score;
                save_high_score();
            }

            set_apple_position();

            // === Обновление уровня и скорости ===
            int new_level = (game_info.score / 50) + 1;
            new_level = std::min(new_level, 10);  // максимум 10

            if (new_level != game_info.level) {
                game_info.level = new_level;

                // Линейное уменьшение задержки от 250 мс (уровень 1) до 100 мс (уровень 10)
                game_info.speed = 250 - (150 * (new_level - 1)) / 9;
            }
        }
        update_game_field();
    }
}

Game_state GameModel::get_state() const {
    return state;
}

void GameModel::set_state(Game_state new_state) {
    state = new_state;
}

Coords GameModel::get_apple_position() const {
    return apple_position;
}

void GameModel::set_apple_position() {
    const auto& body = snake.get_body();
    const int max_attempts = 100;

    // Попробуем случайно (с ограничением)
    for (int i = 0; i < max_attempts; ++i) {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;

        bool occupied = false;
        for (const auto& seg : body) {
            if (seg.x == x && seg.y == y) {
                occupied = true;
                break;
            }
        }

        if (!occupied) {
            apple_position = {x, y};
            return;
        }
    }

    // Если не получилось — ищем первую свободную
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            bool occupied = false;
            for (const auto& seg : body) {
                if (seg.x == x && seg.y == y) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) {
                apple_position = {x, y};
                return;
            }
        }
    }

    // На крайний случай
    apple_position = {0, 0};
}

int GameModel::get_speed() const {
    return game_info.speed;
}

static const char* HIGH_SCORE_FILE = "high_score.txt";

int GameModel::load_high_score() {
    std::ifstream file(HIGH_SCORE_FILE);
    int loaded_score = 0;
    if (file.is_open()) {
        std::string line;
        std::getline(file, line);
        std::stringstream ss(line);
        ss >> loaded_score;
        file.close();
    }
    return loaded_score;
}

void GameModel::save_high_score() {
    std::ofstream file(HIGH_SCORE_FILE);
    if (file.is_open()) {
        file << game_info.high_score;
        file.close();
    }
}


bool GameModel::is_speed_boost_active() const {
    return is_speed_boost_active_;
}