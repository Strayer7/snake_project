#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 10
#define HEIGHT 20
#define RATIO 2
#define STARTING_POINT_FIG_Y 0
#define STARTING_POINT_FIG_X 3
#define FIGURE_SIZE 4

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int x;
  int y;
  int type;
  int rotation;
  int shape[FIGURE_SIZE][FIGURE_SIZE];
} Figure;

typedef struct {
  int **field;
  int **next;
  int score;
  int speed;
  int high_score;
  int level;
  int pause;
} GameInfo_t;

#endif