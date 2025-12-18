#include <iostream>

#include "app.hpp"

int main() {
  ve::Application app;
  try {
    app.mainLoop();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}