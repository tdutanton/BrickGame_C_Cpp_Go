#include "s21_snake_test.h"

TEST(snake, change_level) {
  FULL_INFO;
  full_info->prepare_start_field();
  ASSERT_EQ(full_info->info.level, 1);
  full_info->info.level = 2;
  GameInfo_t info = updateCurrentState();
  ASSERT_EQ(info.level, 2);
  ASSERT_EQ(info.score, 0);
  ASSERT_EQ(info.pause, 0);
  ASSERT_EQ(info.speed, 500);
}
TEST(snake, start) {
  FULL_INFO;
  full_info->state = START_STATE;
  userInput(Start, 0);
  full_info->fsm();
  ASSERT_EQ(full_info->state, SPAWN_STATE);
}

TEST(snake, set_level) {
  FULL_INFO;
  full_info->info.score = 10;
  full_info->set_level();
  ASSERT_EQ(full_info->info.level, 3);
  full_info->info.score = 100;
  full_info->set_level();
  ASSERT_EQ(full_info->info.level, 10);
}

TEST(snake, directions) {
  FULL_INFO;
  full_info->current_action = Right;
  full_info->get_direction();
  full_info->fsm();
  ASSERT_EQ(full_info->direction_, RIGHT);
  full_info->current_action = Down;
  full_info->get_direction();
  full_info->fsm();
  ASSERT_EQ(full_info->direction_, DOWN);
  full_info->current_action = Left;
  full_info->get_direction();
  full_info->fsm();
  ASSERT_EQ(full_info->direction_, LEFT);
  full_info->current_action = Up;
  full_info->get_direction();
  full_info->fsm();
  ASSERT_EQ(full_info->direction_, UP);
  full_info->current_action = Pause;
  full_info->get_direction();
  full_info->fsm();
  ASSERT_EQ(full_info->info.pause, 0);
}

TEST(snake, speed) {
  FULL_INFO;
  full_info->info.level = 1;
  full_info->set_speed();
  ASSERT_EQ(full_info->info.speed, START_SPEED);
}

TEST(snake, random_apple) {
  FULL_INFO;
  full_info->spawn_apple();
  ASSERT_EQ(full_info->info.field[full_info->apple_.x][full_info->apple_.y],
            ATTACHED_BLOCK);
}

TEST(snake, pause) {
  FULL_INFO;
  full_info->pause_process();
  full_info->fsm();
  ASSERT_EQ(full_info->state, PAUSE_STATE);
  full_info->pause_process();
  full_info->fsm();
  ASSERT_NE(full_info->state, PAUSE_STATE);
}

TEST(snake, step) {
  FULL_INFO;
  full_info->snake_step();
  ASSERT_EQ(full_info->snake_body_.size(), 4);
}

TEST(snake, spawn) {
  FULL_INFO;
  full_info->state = MOVING_STATE;
  ASSERT_EQ(full_info->state, MOVING_STATE);
}

TEST(snake, terminate) {
  FULL_INFO;
  full_info->state = START_STATE;
  userInput(Terminate, 0);
  full_info->fsm();
  ASSERT_EQ(full_info->state, TERMINATE_STATE);
}

TEST(snake, terminate_2) {
  FULL_INFO;
  full_info->state = PAUSE_STATE;
  userInput(Terminate, 0);
  full_info->fsm();
  ASSERT_EQ(full_info->state, TERMINATE_STATE);
}

TEST(snake, terminate_3) {
  FULL_INFO;
  full_info->state = MOVING_STATE;
  userInput(Terminate, 0);
  full_info->fsm();
  ASSERT_EQ(full_info->state, TERMINATE_STATE);
}

TEST(snake, restart) {
  FULL_INFO;
  full_info->state = GAME_OVER_STATE;
  userInput(Start, 0);
  full_info->fsm();
  ASSERT_EQ(full_info->state, START_STATE);
}
