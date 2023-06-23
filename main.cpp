#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>

#include "app.hpp"

int main() {
  ve::Application app;
  try {
    app.mainLoop();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}