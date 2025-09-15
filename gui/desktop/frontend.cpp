// frontend.cpp
#include "frontend.h"
#include "../back/controller.h"  // ← Объявляет userInput, updateCurrentState, get_game_model
#include "../back/model.h"      // ← Нужен, чтобы знать GameModel и get_game_model()

game_field::game_field(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // Таймер обновления логики и отрисовки
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &game_field::update_paint);
    timer_->start(250);  // Временно, будет обновляться в update_paint
}

void game_field::keyPressEvent(QKeyEvent *event) {
    bool hold = true;

    UserAction_t action = Start;
    bool handled = true;

    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:    action = Up; break;
        case Qt::Key_Down:
        case Qt::Key_S:    action = Down; break;
        case Qt::Key_Left:
        case Qt::Key_A:    action = Left; break;
        case Qt::Key_Right:
        case Qt::Key_D:    action = Right; break;
        case Qt::Key_P:    action = Pause; hold = false; break;
        case Qt::Key_Escape: action = Terminate; hold = false; break;
        case Qt::Key_Return:
        case Qt::Key_Enter: action = Start; hold = false; break;
        case Qt::Key_Space: action = Action; hold = true; break;  // Ускорение при удержании
        default: handled = false;
    }

    if (handled) {
        userInput(action, hold);
    } else {
        QWidget::keyPressEvent(event);
    }
}

void game_field::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        userInput(Action, false);  // Отпустили пробел — ускорение выключается
    }
    QWidget::keyReleaseEvent(event);
}

void game_field::update_paint() {
    GameInfo_t info = updateCurrentState();

    // Определяем текущую скорость
    int base_speed = info.speed;
    int current_speed = base_speed;

    // Если активировано ускорение — уменьшаем задержку в 1.5 раза
    if (info.pause == 0 && get_game_model().is_speed_boost_active()) {
        current_speed = static_cast<int>(base_speed / 1.5);
        current_speed = std::max(50, current_speed);  // Минимум 50 мс — чтобы не было слишком быстро
    }

    // Обновляем таймер
    if (timer_->interval() != current_speed) {
        timer_->setInterval(current_speed);
    }

    repaint();
}

void game_field::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), QBrush(Qt::darkBlue));  // Фон

    // Получаем текущее состояние игры
    const GameInfo_t& info = *get_game_model().get_game_info();  // Текущее состояние
    Game_state state = get_game_model().get_state();             // Текущее состояние игры

    const int cell_size = 40;
    const int field_width = WIDTH * cell_size;   // 400
    const int field_height = HEIGHT * cell_size; // 800

    // === 1. Рисуем игровое поле (слева) ===
    if (info.field != nullptr) {
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                int value = info.field[y][x];
                QRect cellRect(x * cell_size, y * cell_size, cell_size, cell_size);

                if (value == 1) {
                    // Тело змейки
                    p.fillRect(cellRect, QBrush(Qt::green));
                } else if (value == 2) {
                    // Голова змейки
                    p.fillRect(cellRect, QBrush(Qt::green));

                    // Глаза
                    qreal eyeRadius = cell_size * 0.15;
                    qreal eyeOffsetX = cell_size * 0.25;
                    qreal eyeOffsetY = cell_size * 0.25;
                    qreal centerX = x * cell_size + cell_size / 2.0;
                    qreal centerY = y * cell_size + cell_size / 2.0;

                    p.setBrush(Qt::black);
                    p.setPen(Qt::NoPen);
                    p.drawEllipse(QPointF(centerX - eyeOffsetX, centerY - eyeOffsetY), eyeRadius, eyeRadius);
                    p.drawEllipse(QPointF(centerX + eyeOffsetX, centerY - eyeOffsetY), eyeRadius, eyeRadius);
                    p.setBrush(Qt::darkGreen);  // Восстанавливаем
                } else if (value == 3) {
                    // Яблоко
                    QPointF center(x * cell_size + cell_size / 2.0, y * cell_size + cell_size / 2.0);
                    qreal radius = cell_size * 0.4;

                    QRadialGradient gradient(center, radius);
                    gradient.setColorAt(0, Qt::red);
                    gradient.setColorAt(1, Qt::darkRed);
                    p.setBrush(QBrush(gradient));
                    p.setPen(Qt::NoPen);
                    p.drawEllipse(center, radius, radius);

                    // Блик
                    QPointF shineCenter(x * cell_size + cell_size * 0.35, y * cell_size + cell_size * 0.35);
                    qreal shineRadius = radius * 0.3;
                    p.setBrush(QBrush(Qt::white));
                    p.setOpacity(0.7);
                    p.drawEllipse(shineCenter, shineRadius, shineRadius);
                    p.setOpacity(1.0);

                    // Листик
                    QPointF leafPoint(x * cell_size + cell_size * 0.7, y * cell_size + cell_size * 0.25);
                    p.setBrush(QBrush(Qt::green));
                    p.drawEllipse(leafPoint, cell_size * 0.15, cell_size * 0.15);
                }
            }
        }
    }

    // === 2. UI-панель (справа: 200px) ===
    int panel_x = field_width;
    int panel_width = WINDOW_WIDTH - panel_x;  // 200
    QRect panel_rect(panel_x, 0, panel_width, WINDOW_HEIGHT);

    p.fillRect(panel_rect, QBrush(QColor(50, 50, 70)));  // Темно-серый фон
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 12, QFont::Bold));
    p.drawText(panel_rect.adjusted(20, 20, 0, 0), "SNAKE GAME by Coffeees");

    p.setFont(QFont("Arial", 12));
    p.drawText(panel_rect.adjusted(20, 70, 0, 0), QString("Score: %1").arg(info.score));
    p.drawText(panel_rect.adjusted(20, 100, 0, 0), QString("High: %1").arg(info.high_score));
    p.drawText(panel_rect.adjusted(20, 130, 0, 0), QString("Level: %1").arg(info.level));
    p.drawText(panel_rect.adjusted(20, 160, 0, 0), QString("Speed: %1 ms").arg(info.speed));

    // Подсказки
    p.setFont(QFont("Arial", 10));
    p.setPen(Qt::lightGray);
    p.drawText(panel_rect.adjusted(20, 220, 0, 0), "Controls:");
    p.drawText(panel_rect.adjusted(20, 240, 0, 0), "W/A/S/D or Arrows");
    p.drawText(panel_rect.adjusted(20, 255, 0, 0), "P — Pause");
    p.drawText(panel_rect.adjusted(20, 270, 0, 0), "Enter — Start");
    p.drawText(panel_rect.adjusted(20, 285, 0, 0), "Esc — Quit");

    // === 3. Центральные сообщения — только в пределах игрового поля ===
    int game_area_width = WIDTH * cell_size;  // 400
    int game_area_x = 0;
    int game_area_y = 0;
    int game_area_height = HEIGHT * cell_size; // 800

    QRect gameRect(game_area_x, game_area_y, game_area_width, game_area_height);

    if (state == STATE_START) {
        p.setPen(Qt::yellow);
        p.setFont(QFont("Arial", 24, QFont::Bold));
        p.drawText(gameRect, Qt::AlignCenter, "Press ENTER\nto Start");
    } else if (state == STATE_GAME_OVER) {
        p.setPen(Qt::red);
        p.setFont(QFont("Arial", 24, QFont::Bold));
        p.drawText(gameRect, Qt::AlignCenter,
                   QString("GAME OVER\n\nScore: %1\n\nPress ENTER").arg(info.score));
    } else if (state == STATE_PLAY && info.pause) {
        p.setPen(Qt::cyan);
        p.setFont(QFont("Arial", 18));
        p.drawText(gameRect, Qt::AlignCenter, "PAUSED");
    }
}