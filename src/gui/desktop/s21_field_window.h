#ifndef S21_FIELD_WINDOW_H
#define S21_FIELD_WINDOW_H

#include <QPainter>
#include <QWidget>
#ifdef TETRIS
#include "../../brick_game/tetris/s21_backend_tetris.h"
#elif defined(SNAKE)
#include "../../brick_game/snake/s21_backend_snake.h"
#elif defined(RACE)
#include "../../brick_game/race/s21_backend_race.h"
#else
#include "../../brick_game/snake/s21_backend_snake.h"
#endif

class s21_field_window : public QWidget {
  Q_OBJECT

 public:
  s21_field_window(QWidget *parent = nullptr, int rows = 0, int cols = 0)
      : QWidget(parent) {
    if (parent) {
      setFixedSize(parent->width(), parent->height());
    }
    rows_ = rows;
    cols_ = cols;
  }
  ~s21_field_window() = default;

 protected:
  void paintEvent(QPaintEvent *event) override;
  void paint_grid(int cell, int c_height, int c_width);
  int cols_;
  int rows_;
};

#endif
