#ifndef S21_BACKEND_SNAKE_C_H
#define S21_BACKEND_SNAKE_C_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ROWS 20
#define COLS 10
#define MAX_FIG_ROWS 4
#define MAX_FIG_COLS 4

typedef enum {
  Pause = 0,
  Start = 1,
  Terminate = 2,
  Left = 3,
  Right = 4,
  Up = 5,
  Down = 6,
  Action = 7,
} UserAction_t;

typedef struct {
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#ifdef __cplusplus
}
#endif

#endif