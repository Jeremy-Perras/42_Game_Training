#include "window.hpp"

#include <stdexcept>
#include <utility>

#include "vulkan/vulkan_core.h"

namespace ve {

  Window::Window(int w, int h, std::string name) : width_(w), height_(h), name_(std::move(name)) {
    initWindow();
  }

  Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  bool Window::shouldClose() { return glfwWindowShouldClose(window_) != 0; }

  void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window_ = glfwCreateWindow(width_, height_, name_.c_str(), nullptr, nullptr);
  }
  void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create surface");
    }
  }

}  // namespace ve