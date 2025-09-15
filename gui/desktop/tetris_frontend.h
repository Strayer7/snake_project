#ifndef TETRIS_FRONTEND_H
#define TETRIS_FRONTEND_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include "../common.h"

#define TETRIS_WINDOW_WIDTH  600
#define TETRIS_WINDOW_HEIGHT 800

class tetris_field : public QWidget
{
    Q_OBJECT

public:
    explicit tetris_field(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void update_paint();

private:
    QTimer *timer_;
};

#endif // TETRIS_FRONTEND_H
