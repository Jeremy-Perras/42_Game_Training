#include "window.hpp"

namespace ve {

  Window::Window() { initWindow(); }

  Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  bool Window::shouldClose() { return glfwWindowShouldClose(window) != 0; }

  void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

}  // namespace ve