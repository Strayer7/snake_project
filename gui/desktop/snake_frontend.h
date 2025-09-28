// snake_frontend.h
#ifndef SNAKE_FRONTEND_H
#define SNAKE_FRONTEND_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include "../../common.h"

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 800

class game_field : public QWidget
{
    Q_OBJECT

public:
    explicit game_field(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void update_paint();

private:
    QTimer *timer_;
};

#endif // FRONTEND_H