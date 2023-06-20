#include "window.hpp"

namespace ve {

  Window::Window() { initWindow(); }

  Window::~Window() {
    glfwDestroyWindow(_window);
    glfwTerminate();
  }

  bool Window::shouldClose() { return glfwWindowShouldClose(_window) != 0; }

  void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
  }

}  // namespace ve