#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <time.h>

#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QPalette>
#include <QTimer>

#include "s21_field_window.h"
#include "s21_next_field_window.h"
#ifdef TETRIS
#include "../../brick_game/tetris/s21_backend_tetris.h"
#elif defined(SNAKE)
#include "../../brick_game/snake/s21_backend_snake.h"
#elif defined(RACE)
#include "../../brick_game/race/s21_backend_race.h"
#else
#include "../../brick_game/snake/s21_backend_snake.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 protected:
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

 private:
  Ui::MainWindow *ui;
  s21_field_window *field_window;
  s21_next_field_window *next_field_window;
  QTimer *timer;
  void print_current_game_state();
  bool paused_;
  bool game_started_;
  QLabel *welcomeLabel;
  bool pause;
};
#endif  // MAINWINDOW_H
