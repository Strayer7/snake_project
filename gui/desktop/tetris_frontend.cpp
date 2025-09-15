#include "tetris_frontend.h"
#include "../back_tetris/backend.h"   // твой backend.c/.h

tetris_field::tetris_field(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(TETRIS_WINDOW_WIDTH, TETRIS_WINDOW_HEIGHT);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &tetris_field::update_paint);
    timer_->start(300); // обновление каждые 300 мс (будет динамически меняться)
}

void tetris_field::keyPressEvent(QKeyEvent *event) {
    bool hold = true;

    switch (event->key()) {
        case Qt::Key_Left:  userInput(Left, hold); break;
        case Qt::Key_Right: userInput(Right, hold); break;
        case Qt::Key_Down:  userInput(Down, hold); break;
        case Qt::Key_Up:    userInput(Action, hold); break; // Поворот
        case Qt::Key_P:     userInput(Pause, false); break;
        case Qt::Key_Escape: userInput(Terminate, false); break;
        case Qt::Key_Return:
        case Qt::Key_Enter: userInput(Start, false); break;
        default: QWidget::keyPressEvent(event); break;
    }
}

void tetris_field::update_paint() {
    GameInfo_t info = updateCurrentState();

    // обновляем частоту таймера под уровень
    if (timer_->interval() != info.speed) {
        timer_->setInterval(info.speed);
    }

    repaint();
}

void tetris_field::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), QBrush(Qt::black));

    GameInfo_t info = updateCurrentState();

    const int cell_size = 30;
    const int field_width = WIDTH * cell_size;
    const int field_height = HEIGHT * cell_size;

    // === Игровое поле ===
    if (info.field != nullptr) {
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                if (info.field[y][x]) {
                    QRect cellRect(x * cell_size, y * cell_size, cell_size, cell_size);
                    p.fillRect(cellRect, QBrush(Qt::cyan));
                    p.setPen(Qt::darkGray);
                    p.drawRect(cellRect);
                }
            }
        }
    }

    // === Next figure ===
    if (info.next != nullptr) {
        int next_x = field_width + 40;
        int next_y = 50;
        for (int i = 0; i < FIGURE_SIZE; ++i) {
            for (int j = 0; j < FIGURE_SIZE; ++j) {
                if (info.next[i][j]) {
                    QRect cellRect(next_x + j * cell_size, next_y + i * cell_size, cell_size, cell_size);
                    p.fillRect(cellRect, QBrush(Qt::yellow));
                    p.setPen(Qt::darkGray);
                    p.drawRect(cellRect);
                }
            }
        }
    }

    // === Панель справа ===
    int panel_x = field_width;
    QRect panelRect(panel_x, 0, TETRIS_WINDOW_WIDTH - panel_x, TETRIS_WINDOW_HEIGHT);
    p.fillRect(panelRect, QBrush(QColor(50, 50, 70)));

    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 12, QFont::Bold));
    p.drawText(panelRect.adjusted(20, 20, 0, 0), "TETRIS GAME");

    p.setFont(QFont("Arial", 12));
    p.drawText(panelRect.adjusted(20, 70, 0, 0), QString("Score: %1").arg(info.score));
    p.drawText(panelRect.adjusted(20, 100, 0, 0), QString("High: %1").arg(info.high_score));
    p.drawText(panelRect.adjusted(20, 130, 0, 0), QString("Level: %1").arg(info.level));
    p.drawText(panelRect.adjusted(20, 160, 0, 0), QString("Speed: %1 ms").arg(info.speed));

    // === Game Over ===
    if (is_game_over()) {
        p.setPen(Qt::red);
        p.setFont(QFont("Arial", 24, QFont::Bold));
        p.drawText(QRect(0, 0, field_width, field_height), Qt::AlignCenter, "GAME OVER");
    }
}
