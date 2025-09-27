#include "mainwindow.h"

#include "ui_mainwindow.h"

/**
 * @brief Constructs a new MainWindow.
 *
 * Sets up the UI and initializes the game state and its components.
 *
 * @param parent The parent widget of this window.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  srand(time(NULL));
  ui->setupUi(this);
  game_started_ = false;
  paused_ = false;
  field_window = new s21_field_window(ui->wField, ROWS, COLS);
  this->setStyleSheet("background-color: #a8c64e;");
  next_field_window =
      new s21_next_field_window(ui->wNext_field, MAX_FIG_ROWS, MAX_FIG_COLS);
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &MainWindow::print_current_game_state);
  timer->start(30);
}

/**
 * @brief Destructs a new MainWindow.
 */
MainWindow::~MainWindow() { delete ui; }

/**
 * @brief Processes key press events for the game window.
 *
 * This function is used to convert Qt key press events to UserAction_t actions
 * that are then passed to the game logic through the userInput function.
 *
 * The following keys are supported:
 * - Enter: Start
 * - P or p: Pause
 * - Q or q: Terminate
 * - Left arrow: Left
 * - Right arrow: Right
 * - Up arrow: Up
 * - Down arrow: Down
 * - Space: Action
 *
 * All other keys are ignored.
 */
void MainWindow::keyPressEvent(QKeyEvent *event) {
  UserAction_t push;
  int key = event->key();
  if (key == Qt::Key_Return) {
    push = Start;
    game_started_ = true;
    userInput(push, false);
  } else if (key == Qt::Key_P || key == 'p') {
    push = Pause;
    paused_ = !paused_;
    userInput(push, false);
  } else if (key == Qt::Key_Q || key == 'q') {
    push = Terminate;
    userInput(push, false);
    QWidget::close();
  } else if (key == Qt::Key_Left) {
    push = Left;
    userInput(push, false);
  } else if (key == Qt::Key_Right) {
    push = Right;
    userInput(push, false);
  } else if (key == Qt::Key_Up) {
    push = Up;
    userInput(push, true);
  } else if (key == Qt::Key_Down) {
    push = Down;
    userInput(push, false);
  } else if (key == Qt::Key_Space) {
    push = Action;
    userInput(push, true);
  } else {
    QMainWindow::keyPressEvent(event);
  }
}

/**
 * @brief Process a key release event.
 *
 * This function is used to process events when a key is released. It
 * checks if the released key is the space bar, and if so, stops the action.
 * Otherwise, it calls QMainWindow's keyReleaseEvent.
 *
 * @param event The key release event.
 */
void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Space)
    userInput(Action, false);
  else if (event->key() == Qt::Key_Up)
    userInput(Up, false);
  else
    QMainWindow::keyReleaseEvent(event);
}

/**
 * @brief Prints the current game state.
 *
 * This function prints the current game state in the corresponding labels
 * and displays of the main window. It also updates the game field and the
 * next field.
 *
 * @details
 * This function is called by the timer to update the game state every 30
 * milliseconds.
 *
 * @see updateCurrentState()
 */
void MainWindow::print_current_game_state() {
  if (!game_started_) {
    return;
  }
  GameInfo_t info = updateCurrentState();
  if (paused_)
    ui->label_pause->show();
  else {
    ui->label_pause->hide();
  }
  if (!game_started_)
    ui->label_welcome->show();
  else {
    ui->label_welcome->hide();
  }
  ui->num_high_score->display(info.high_score);
  ui->num_score->display(info.score);
  ui->num_level->display(info.level);
  ui->num_speed->display(info.level);
  ui->wField->update();
  ui->wNext_field->update();
}
