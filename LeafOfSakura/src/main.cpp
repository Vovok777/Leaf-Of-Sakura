#include "Game.h"
#include <iostream>
#include <cstdlib>

int main() {
  system("chcp 65001 > nul");

  Game game("data");
  game.Run();

  std::cout << "\n  Нажми Enter для выхода...\n";
  std::cin.ignore(10000, '\n');
  return 0;
}
