/**
 * \file
 * \brief Header file for Tetris frontend
 *
 * Contains a description of the main data types and functions used for
 * presentation game in console screen
 */

#ifndef S21_FRONTEND
#define S21_FRONTEND

#ifdef TETRIS
#include "../../brick_game/tetris/s21_backend_tetris.h"
#define L_BLOCK '['
#define R_BLOCK ']'
#define L_ATTACHED '['
#define R_ATTACHED ']'
#elif defined(SNAKE)
#include "../../brick_game/snake/s21_backend_snake.h"
#define L_BLOCK '('
#define R_BLOCK ')'
#define L_ATTACHED '['
#define R_ATTACHED ']'
#elif defined(RACE)
#include "../../brick_game/race/s21_backend_race.h"
#define L_BLOCK '('
#define R_BLOCK ')'
#define L_ATTACHED '['
#define R_ATTACHED ']'
#else
#include "../../brick_game/tetris/s21_backend_tetris.h"
#define L_BLOCK '['
#define R_BLOCK ']'
#define L_ATTACHED '['
#define R_ATTACHED ']'
#endif

#define COLS_FRONT 20

typedef WINDOW* WIN;

void paint_boxes(void);
void print_stats(GameInfo_t info);
void print_greeting(void);
void fill_main_field(GameInfo_t info);
void fill_next_figure(GameInfo_t info);
void refresh_windows(void);
void print_current_game_state(GameInfo_t info);
void print_game_over(GameInfo_t info);
void print_pause(void);
void print_terminate(GameInfo_t info);

#endif