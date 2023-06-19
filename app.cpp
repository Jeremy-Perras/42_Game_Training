#include "app.hpp"

namespace ve {
  void Application::run() {
    initVulkan();
    mainLoop();
  }

  void Application::mainLoop() {
    while (static_cast<int>(window.shouldClose()) == 0) {
      glfwPollEvents();
    }
  }

  void Application::initVulkan(){};

}  // namespace ve