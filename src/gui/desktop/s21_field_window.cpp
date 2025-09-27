#include "s21_field_window.h"

/**
 * @brief Draws the game field
 *
 * This function is called automatically by Qt when the window needs to be
 * redrawn.
 *
 * It draws the grid of the game field, then draws the current figure and the
 * attached blocks.
 */
void s21_field_window::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  int clean_height = height() - 10;
  int clean_width = width() - 10;
  int cell = clean_height / rows_;
  paint_grid(cell, clean_height, clean_width);
  GameInfo_t info = updateCurrentState();
  if (info.field == nullptr) return;
  QPainter painter(this);
  for (int i = 0; i < rows_; i++) {
    for (int j = 0; j < cols_; j++) {
      if (info.field[i][j] == CURRENT_FIGURE_BLOCK) {
        QRect cellRect(j * cell, i * cell, cell, cell);
        painter.setBrush(Qt::black);
        painter.drawRect(cellRect);
      }
      if (info.field[i][j] == ATTACHED_BLOCK) {
        QRect cellRect(j * cell, i * cell, cell, cell);
        painter.setBrush(Qt::black);
        painter.drawRect(cellRect);
      }
    }
  }
}

/**
 * @brief Draws the grid of the game field
 *
 * Draws vertical and horizontal lines with the step equal to cell size.
 * The grid is drawn on the whole window.
 *
 * @param cell - cell size
 * @param c_height - full height of the window
 * @param c_width - full width of the window
 */
void s21_field_window::paint_grid(int cell, int c_height, int c_width) {
  QPainter painter(this);
  QPen pen(Qt::black);
  pen.setColor(QColor(0, 0, 0, 128));
  painter.setPen(pen);
  for (int i = 0, g = 0; i <= cols_; i++, g += cell) {
    painter.drawLine(g, 0, g, c_height);
  }
  for (int i = 0, g = 0; i <= rows_; i++, g += cell) {
    painter.drawLine(0, g, c_width, g);
  }
}
