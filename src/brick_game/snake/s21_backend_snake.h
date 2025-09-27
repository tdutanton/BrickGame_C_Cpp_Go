#ifndef S21_SNAKE_MODEL
#define S21_SNAKE_MODEL

#include <ncurses.h>
#include <sys/time.h>

#include <cstdlib>
#include <ctime>
#include <deque>
#include <utility>

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
#define FULL_INFO s21::SnakeGame* full_info = s21::SnakeGame::update_full_info()

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
  int x, y;
} APPLE;

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT,
} DIRECTION;

namespace s21 {

class SnakeGame {
#ifdef TEST
 public:
#endif
  APPLE apple_;
  std::deque<std::pair<int, int>> snake_body_;
  DIRECTION direction_;
  bool is_bad_move(int x, int y, int d_y);
  void check_eat(int x, int y);
  void set_speed();
  void pause_process();
  void get_direction();
  void spawn_apple();
  void start_actions();
  void prepare_start_field();
  void set_level();
  void place_snake_on_field();
  void copy_only_apple();
  void snake_step();
  long long update_time();
  void update_high_score();
  void read_high_score();
  void clean_field();

 public:
  SnakeGame();
  ~SnakeGame() /* = default*/;

  GameInfo_t info;
  int current_action;
  long long timer_;
  bool holding_;
  game_state state;
  static SnakeGame* update_full_info();
  void fsm();
};

}  // namespace s21

#ifdef __cplusplus
extern "C" {
#endif

int** init_empty_area(int rows, int cols);
void copy_area(int** current, int** new_area, int rows, int cols);
GameInfo_t updateCurrentState(void);
void userInput(UserAction_t action, bool hold);

#ifdef __cplusplus
}
#endif

#endif