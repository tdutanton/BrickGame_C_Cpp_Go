#include "s21_backend_snake.h"

#include <iostream>

/**
 * Constructor for SnakeGame class
 *
 * Initializes the object with default values:
 * - apple_ is empty
 * - snake_body_ is empty
 * - state is START_STATE
 * - timer_ is 0
 * - direction_ is set to UP
 * - info is empty
 */
s21::SnakeGame::SnakeGame() {
  apple_ = {};
  snake_body_ = {};
  state = START_STATE;
  current_action = Pause;
  timer_ = 0;
  holding_ = false;
  direction_ = DIRECTION::UP;
  info = {};
}

/**
 * Destructor for SnakeGame class
 *
 * Deletes the dynamically allocated memory for the game field and next figure
 * when the object is destroyed.
 */
s21::SnakeGame::~SnakeGame() {
  FULL_INFO;
  if (full_info->info.field) {
    for (int i = 0; i < ROWS; i++) {
      delete[] full_info->info.field[i];
    }
    delete[] full_info->info.field;
  }
  if (full_info->info.next) {
    for (int i = 0; i < MAX_FIG_ROWS; i++) {
      delete[] full_info->info.next[i];
    }
    delete[] full_info->info.next;
  }
}

int next_apple[][4] = {{0, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

/**
 * Returns a pointer to the current state of the game.
 *
 * This function is used as a singleton to access the current state of the game.
 * It initializes the game field and next figure on the first call and then
 * returns the same pointer to the same object on subsequent calls.
 *
 * @return A pointer to the current state of the game.
 */
s21::SnakeGame* s21::SnakeGame::update_full_info() {
  static SnakeGame Game{};
  if (Game.info.field == nullptr) Game.info.field = init_empty_area(ROWS, COLS);
  if (Game.info.next == nullptr)
    Game.info.next = init_empty_area(MAX_FIG_ROWS, MAX_FIG_COLS);
  return &Game;
}

/**
 * Initializes a 2D array with the given number of rows and columns and fills
 * it with EMPTY_BLOCK.
 *
 * @param rows The number of rows in the array.
 * @param cols The number of columns in the array.
 * @return A pointer to a 2D array with the given number of rows and columns.
 */
int** init_empty_area(int rows, int cols) {
  int** empty_field = new int*[rows];
  for (int i = 0; i < rows; ++i) {
    empty_field[i] = new int[cols];
    std::fill_n(empty_field[i], cols, EMPTY_BLOCK);
  }
  return empty_field;
}

/**
 * Copies the content of a 2D array to another 2D array.
 *
 * This function is used to copy the content of a 2D array, typically a game
 * field or a figure, to another 2D array.
 *
 * @param current The 2D array to copy from.
 * @param new_area The 2D array to copy to.
 * @param rows The number of rows in the 2D array.
 * @param cols The number of columns in the 2D array.
 */
void copy_area(int** current, int** new_area, int rows, int cols) {
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++) {
      new_area[i][j] = current[i][j];
    }
}

/**
 * Main game function. Returns a current state of the structure from pointer of
 * the structure with all parameters
 */
GameInfo_t updateCurrentState(void) {
  FULL_INFO;
  full_info->fsm();

  return full_info->info;
}

/**
 * Recognition of pressed keys, changing current states depending on the pressed
 * key \param action code of key that user push on keyboard \param hold
 * represent of holding a button
 */
void userInput(UserAction_t action, bool hold) {
  FULL_INFO;
  full_info->current_action = action;
  if (action == Action) full_info->holding_ = hold;
}

/**
 * Finite state machine that controls the game flow.
 *
 * The state machine runs every time the user inputs a key. It checks the
 * current state and runs the corresponding actions. The actions are:
 * - START_STATE: start the game, place the snake on the field and start the
 *   timer.
 * - SPAWN_STATE: spawn an apple and move to MOVING_STATE.
 * - MOVING_STATE: move the snake, check for collisions and update the score.
 * - PAUSE_STATE: pause the game, and wait for the user to unpause or terminate.
 * - GAME_OVER_STATE: game over, wait for the user to restart or terminate.
 *
 * The state machine is also responsible for updating the timer and the score.
 */
void s21::SnakeGame::fsm() {
  switch (state) {
    case START_STATE:
      start_actions();
      place_snake_on_field();
      break;
    case SPAWN_STATE:
      spawn_apple();
      state = MOVING_STATE;
      break;
    case MOVING_STATE:
      get_direction();
      if (current_action == Action && !holding_) timer_ -= (info.level * 50);
      if ((update_time() - timer_) >= (info.speed / (1 + holding_))) {
        timer_ = update_time();
        snake_step();
      }
      break;
    case PAUSE_STATE:
      switch (current_action) {
        case Pause:
          pause_process();
          break;
        case Terminate:
          update_high_score();
          state = TERMINATE_STATE;
          break;
        default:
          break;
      }
      break;
    case GAME_OVER_STATE:
      switch (current_action) {
        case Start:
          state = START_STATE;
          break;
        default:
          break;
      }
    default:
      break;
  }
  current_action = -1;
}

/**
 * Proccessing of start actions depending on keyboard keys (user actions)
 * \param action code of key that user push on keyboard
 */
void s21::SnakeGame::start_actions() {
  switch (current_action) {
    case Start:
      prepare_start_field();
      state = SPAWN_STATE;
      break;
    case Terminate:
      state = TERMINATE_STATE;
      break;
    default:
      state = START_STATE;
      break;
  }
}

/**
 * Reset game to start state
 *
 * Reset apple position and clear snake body
 * Set default values for game state and score
 * Reset game speed and set pause flag
 * Set default value for next area
 * Push default values to snake body
 * Clean game field
 */
void s21::SnakeGame::prepare_start_field() {
  apple_.x = 0;
  apple_.y = 0;
  snake_body_.clear();
  direction_ = {};
  read_high_score();
  info.level = 1;
  info.score = 0;
  info.pause = 0;
  info.speed = START_SPEED;
  info.next[1][1] = CURRENT_FIGURE_BLOCK;
  for (int i = 6; i <= 9; i++) {
    snake_body_.push_back({4, i});
  }
  clean_field();
}

/**
 * Clear the game field from all objects
 *
 * Reset all elements of 2D array to EMPTY_BLOCK
 */
void s21::SnakeGame::clean_field() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      info.field[i][j] = EMPTY_BLOCK;
    }
  }
}

/**
 * Read high score from file and set it to game info
 *
 * Try to open file and read high score from it
 * If file is not found, set high score to 0
 */
void s21::SnakeGame::read_high_score() {
  int high_score = -1;
  FILE* f = fopen("snake_high_score.txt", "r");
  if (f) {
    if (fscanf(f, "%d", &high_score) != EOF) {
      info.high_score = high_score;
    } else {
      info.high_score = 0;
    }
    fclose(f);
  } else {
    info.high_score = 0;
  }
}

/**
 * Update (rewrite) new high score
 *
 * Try to open file and write current high score to it
 * If file is not found, do nothing
 */
void s21::SnakeGame::update_high_score() {
  FILE* f = fopen("snake_high_score.txt", "w");
  if (f) {
    fprintf(f, "%d", info.high_score);
    fclose(f);
  }
}

/**
 * Place snake on the game field
 *
 * Update time of the snake movement and draw snake on the game field
 */
void s21::SnakeGame::place_snake_on_field() {
  timer_ = update_time();
  for (size_t i = 0; i < snake_body_.size(); i++) {
    info.field[snake_body_[i].second][snake_body_[i].first] =
        CURRENT_FIGURE_BLOCK;
  }
}

/**
 * Get current time. Uses for shifting Figure
 *
 * Return current time in milliseconds
 */
long long s21::SnakeGame::update_time() {
  struct timeval current;
  gettimeofday(&current, nullptr);
  return ((long long)current.tv_sec * 1000 + current.tv_usec / 1000);
}

/**
 * Spawn new apple on the game field
 *
 * Generate random position for new apple and check this position
 * for emptiness. If position is not empty, generate new position
 * and check again. After finding empty position, draw new apple on
 * the game field.
 */
void s21::SnakeGame::spawn_apple() {
  do {
    apple_.x = rand() % ROWS;
    apple_.y = rand() % COLS;
  } while (info.field[apple_.x][apple_.y] != EMPTY_BLOCK);
  info.field[apple_.x][apple_.y] = ATTACHED_BLOCK;
}

/**
 * Handle user action and set direction for snake movement
 *
 * Change direction of snake body if it is possible
 * Change game state to PAUSE_STATE if Pause action is received
 * Change game state to TERMINATE_STATE if Terminate action is received
 * Update high score if Terminate action is received
 */
void s21::SnakeGame::get_direction() {
  switch (current_action) {
    case Up:
      if (direction_ != DOWN) direction_ = UP;
      break;
    case Down:
      if (direction_ != UP) direction_ = DOWN;
      break;
    case Left:
      if (direction_ != RIGHT) direction_ = LEFT;
      break;
    case Right:
      if (direction_ != LEFT) direction_ = RIGHT;
      break;
    case Action:
      break;
    case Pause:
      pause_process();
      break;
    case Terminate:
      state = TERMINATE_STATE;
      update_high_score();
      break;
    default:
      break;
  }
}

/**
 * Set current game level depending on a current number of scores
 *
 * Increase the game level and limit it to 10 levels
 */
void s21::SnakeGame::set_level() {
  info.level =
      (info.score >= LEVEL_STEP ? (info.score / (int)LEVEL_STEP) + 1 : 1);
  if (info.level > 10) info.level = 10;
}

/**
 * Make one step of snake movement
 *
 * Update snake position and check if it doesn't intersect with itself or game
 * field If intersection is found, change game state to GAME_OVER_STATE Check if
 * snake eats apple, add new apple and update score If snake body size is more
 * than 200, change game state to GAME_OVER_STATE Copy only apple to game field
 * Place snake on the game field
 */
void s21::SnakeGame::snake_step() {
  int delta_x = 0;
  int delta_y = 0;
  switch (direction_) {
    case LEFT:
      delta_x = -1;
      break;
    case RIGHT:
      delta_x = 1;
      break;
    case UP:
      delta_y = -1;
      break;
    case DOWN:
      delta_y = 1;
      break;
  }
  int new_head_x = snake_body_.front().first + delta_x;
  int new_head_y = snake_body_.front().second + delta_y;
  if (is_bad_move(new_head_x, new_head_y, delta_y))
    state = GAME_OVER_STATE;
  else {
    snake_body_.push_front({new_head_x, new_head_y});
    check_eat(new_head_x, new_head_y);
    if (snake_body_.size() >= 200) state = GAME_OVER_STATE;
    copy_only_apple();
    place_snake_on_field();
  }
}

/**
 * Check if new snake head position is bad move
 *
 * Check if new snake head position is out of game field
 * or if it intersects with snake body
 *
 * @param[in] x new snake head x position
 * @param[in] y new snake head y position
 * @param[in] d_y direction of snake body growth
 *
 * @return true if new snake head position is bad move, false otherwise
 */
bool s21::SnakeGame::is_bad_move(int x, int y, int d_y) {
  bool result = false;
  for (size_t i = 1; i < snake_body_.size(); i++) {
    if (x == snake_body_[i].first && y == snake_body_[i].second) {
      result = true;
    }
  }
  if (!result && (x == -1 || x == COLS)) {
    result = true;
  } else if (!result && ((y == -1 && d_y == -1) || (y == ROWS && d_y == 1))) {
    result = true;
  }
  return result;
}

/**
 * Check if snake eats apple
 *
 * Check if snake head position is equal to apple position.
 * If it is, increase score, update high score if necessary,
 * set new level and speed, and change game state to SPAWN_STATE.
 * If not, remove last element from snake body.
 *
 * @param[in] x snake head x position
 * @param[in] y snake head y position
 */
void s21::SnakeGame::check_eat(int x, int y) {
  if (x == apple_.y && y == apple_.x) {
    info.score++;
    if (info.score > info.high_score) {
      info.high_score = info.score;
      update_high_score();
    }
    set_level();
    set_speed();
    state = SPAWN_STATE;
  } else {
    snake_body_.pop_back();
  }
}

/**
 * Copy only apple to game field
 *
 * Clear all game field except attached block that represents apple
 */
void s21::SnakeGame::copy_only_apple() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      info.field[i][j] =
          (info.field[i][j] == ATTACHED_BLOCK) ? ATTACHED_BLOCK : EMPTY_BLOCK;
    }
  }
}

/**
 * Change pause state
 *
 * Change pause state depending on current state of the game.
 * If game is not paused, pause it and change state to PAUSE_STATE.
 * If game is paused, unpause it and change state to MOVING_STATE.
 */
void s21::SnakeGame::pause_process() {
  // FULL_INFO;
  /*   info.pause = !info.pause;
    state = (info.pause) ? PAUSE_STATE : MOVING_STATE; */
  if (!info.pause) {
    info.pause = 1;
    state = PAUSE_STATE;
  } else if (info.pause) {
    info.pause = 0;
    state = MOVING_STATE;
  }
}

/**
 * Set current game speed depending on a current game level
 *
 * Decrease game speed by 50 milliseconds for each game level
 * if current level is greater than 1
 */
void s21::SnakeGame::set_speed() {
  info.speed =
      (info.level > 1 ? START_SPEED - 50 * (info.level - 1) : START_SPEED);
}