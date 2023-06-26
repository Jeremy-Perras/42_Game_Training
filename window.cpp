#include "window.hpp"

#include <sstream>
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window_ = glfwCreateWindow(width_, height_, name_.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
  }

  void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create surface");
    }
  }

  void Window::updateFrame(unsigned int fps) {
    std::stringstream newTitle;

    newTitle << "GameEngine"
             << " "
             << " [" << fps << " FPS]";

    glfwSetWindowTitle(window_, newTitle.str().c_str());
  }

  void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto *newWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    newWindow->frameBufferResized_ = true;
    newWindow->width_ = width;
    newWindow->height_ = height;
  }

}  // namespace ve