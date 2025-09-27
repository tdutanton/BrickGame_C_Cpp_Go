#include "s21_tetris_test.h"

START_TEST(change_level) {
  FULL_INFO;
  prepare_start_field();
  full_info->info.level = 2;
  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.level, 2);
  ck_assert_int_eq(info.score, 0);
  ck_assert_int_eq(info.pause, 0);
  ck_assert_int_eq(info.speed, 1400);
}
END_TEST

START_TEST(start) {
  FULL_INFO;
  full_info->state = START_STATE;
  userInput(Start, 0);
  ck_assert_int_eq(full_info->state, START_STATE);
}
END_TEST

START_TEST(start_2) {
  FULL_INFO;
  full_info->state = START_STATE;
  userInput(-1, 0);
  ck_assert_int_eq(full_info->state, START_STATE);
}
END_TEST

START_TEST(spawn) {
  FULL_INFO;
  full_info->state = MOVING_STATE;
  ck_assert_int_eq(full_info->state, MOVING_STATE);
}
END_TEST

START_TEST(terminate) {
  FULL_INFO;
  full_info->state = START_STATE;
  userInput(Terminate, 0);
  fsm();
  ck_assert_int_eq(full_info->state, TERMINATE_STATE);
}
END_TEST

END_TEST

START_TEST(terminate_2) {
  FULL_INFO;
  full_info->state = PAUSE_STATE;
  userInput(Terminate, 0);
  fsm();
  ck_assert_int_eq(full_info->state, TERMINATE_STATE);
}
END_TEST

START_TEST(attaching) {
  FULL_INFO;
  full_info->state = MOVING_STATE;
  spawn_figure();
  userInput(Down, 0);
  fsm();
  ck_assert_int_eq(full_info->state, ATTACHING_STATE);
}
END_TEST

START_TEST(step_left_test) {
  FULL_INFO;
  full_info->state = MOVING_STATE;
  full_info->figure.x = 1;
  userInput(Left, 0);
  fsm();
  ck_assert_int_eq(full_info->figure.x, 0);
}
END_TEST

START_TEST(step_right_test) {
  FULL_INFO;
  full_info->state = MOVING_STATE;
  full_info->figure.x = 1;
  userInput(Right, 0);
  fsm();
  ck_assert_int_eq(full_info->figure.x, 2);
}
END_TEST

START_TEST(restart) {
  FULL_INFO;
  full_info->state = GAME_OVER_STATE;
  userInput(Start, 0);
  fsm();
  ck_assert_int_eq(full_info->state, START_STATE);
}
END_TEST

START_TEST(attaching_2) {
  FULL_INFO;
  full_info->state = SHIFTING_STATE;
  userInput(-1, 0);
  fsm();
  ck_assert_int_eq(full_info->state, ATTACHING_STATE);
}
END_TEST

START_TEST(attaching_3) {
  FULL_INFO;
  full_info->state = ATTACHING_STATE;
  userInput(-1, 0);
  fsm();
  ck_assert_int_eq(full_info->state, SPAWN_STATE);
}
END_TEST

START_TEST(attaching_4) {
  FULL_INFO;
  full_info->state = ATTACHING_STATE;
  for (int i = 0; i < COLS; i++) {
    full_info->info.field[i][1] = ATTACHED_BLOCK;
  }
  userInput(-1, 0);
  fsm();
  ck_assert_int_eq(full_info->state, GAME_OVER_STATE);
}
END_TEST

START_TEST(rotate_test_I) {
  FULL_INFO;
  prepare_start_field();
  spawn_figure();
  full_info->figure.fig_name = 'I';
  ck_assert_int_eq(full_info->info.field[0][5], EMPTY_BLOCK);
  rotate();
  ck_assert_int_eq(full_info->info.field[0][3], EMPTY_BLOCK);
  rotate();
  ck_assert_int_eq(full_info->info.field[1][5], EMPTY_BLOCK);
}

START_TEST(rotate_test_S_Z) {
  FULL_INFO;
  prepare_start_field();
  spawn_figure();
  full_info->figure.fig_name = 'S';
  ck_assert_int_eq(full_info->info.field[0][5], CURRENT_FIGURE_BLOCK);
  rotate();
  ck_assert_int_eq(full_info->info.field[0][3], EMPTY_BLOCK);
  rotate();
  ck_assert_int_eq(full_info->info.field[1][5], CURRENT_FIGURE_BLOCK);
  free_field_memory();
}

START_TEST(update_score_test) {
  ck_assert_int_eq(updated_score(1), 100);
  ck_assert_int_eq(updated_score(2), 300);
  ck_assert_int_eq(updated_score(3), 700);
  ck_assert_int_eq(updated_score(4), 1500);
}
END_TEST

Suite *test_s21_tetris() {
  Suite *suite = suite_create("test_s21_tetris");
  TCase *t_case = tcase_create("test_s21_tetris");

  suite_add_tcase(suite, t_case);
  tcase_add_test(t_case, change_level);
  tcase_add_test(t_case, start);
  tcase_add_test(t_case, start_2);
  tcase_add_test(t_case, spawn);
  tcase_add_test(t_case, terminate);
  tcase_add_test(t_case, terminate_2);
  // tcase_add_test(t_case, terminate_3);
  tcase_add_test(t_case, attaching);
  tcase_add_test(t_case, attaching_2);
  tcase_add_test(t_case, step_left_test);
  tcase_add_test(t_case, step_right_test);
  tcase_add_test(t_case, attaching_3);
  tcase_add_test(t_case, attaching_4);
  tcase_add_test(t_case, restart);
  // tcase_add_test(t_case, rotate_test);
  tcase_add_test(t_case, rotate_test_I);
  tcase_add_test(t_case, rotate_test_S_Z);
  tcase_add_test(t_case, update_score_test);
  return suite;
}