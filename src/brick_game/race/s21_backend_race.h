#ifndef S21_BACKEND_RACE_H
#define S21_BACKEND_RACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#define SPACEBAR 32
#define EMPTY_BLOCK 0
#define CURRENT_FIGURE_BLOCK 1
#define ATTACHED_BLOCK 2
#define ROWS 20
#define COLS 10
#define LEVEL_STEP 5
#define MAX_FIG_ROWS 4
#define MAX_FIG_COLS 4
#define START_SPEED 500
#define ENTER_KEY 10
#define FULL_INFO full_game_info_t* full_info = update_ptr_full()

typedef enum {
  Pause,
  Start,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action,
} UserAction_t;

typedef enum {
  START_STATE,
  SPAWN_STATE,
  MOVING_STATE,
  SHIFTING_STATE,
  ATTACHING_STATE,
  GAME_OVER_STATE,
  PAUSE_STATE,
  TERMINATE_STATE,
} game_state;

typedef struct {
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef struct {
  int X;
  int Y;
} Car;

typedef struct full_game_info_t {
  GameInfo_t info;
  Car Player;
  game_state state;
  int holding_;
  UserAction_t current_action;
} full_game_info_t;

GameInfo_t updateCurrentState(void);
void userInput(UserAction_t action, uint8_t hold);
full_game_info_t* update_ptr_full(void);

#ifdef __cplusplus
}
#endif

#endif  // S21_BACKEND_RACE_H