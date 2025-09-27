#include <cstdlib>
#include <iostream>

int main() {
  char choice;
  std::cout << "Games:" << std::endl;
  std::cout << "1. Tetris console" << std::endl;
  std::cout << "2. Snake console" << std::endl;
  std::cout << "3. Race console" << std::endl;
  std::cout << "4. Tetris desktop" << std::endl;
  std::cout << "5. Snake desktop" << std::endl;
  std::cout << "6. Race desktop" << std::endl;
  std::cout << "7. Tetris web (go to localhost:8080)" << std::endl;
  std::cout << "8. Snake web (go to localhost:8080)" << std::endl;
  std::cout << "9. Race web (go to localhost:8080)" << std::endl;
  std::cout << "Q - quit" << std::endl;

  std::cout << "Make your choice: ";
  std::cin >> choice;

  switch (choice) {
    case '1':
      system("./tetris");
      break;
    case '2':
      system("./snake");
      break;
    case '3':
      system("./race");
      break;
    case '4':
      system("gui/desktop/s21_tetris_desktop");
      break;
    case '5':
      system("gui/desktop/s21_snake_desktop");
      break;
    case '6':
      system("gui/desktop/s21_race_desktop");
      break;
    case '7':
      system("./tetris_web_game");
      break;
    case '8':
      system("./snake_web_game");
      break;
    case '9':
      system("./race_web_game");
      break;
    case 'q':
    case 'Q':
      std::cout << "Good bye!" << std::endl;
      break;
    default:
      std::cout << "Wrong choice. Try again!" << std::endl;
      break;
  }

  return 0;
}