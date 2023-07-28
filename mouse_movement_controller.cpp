#include "mouse_movement_controller.hpp"

#include <iostream>

namespace ve {
  MouseMovementController::MouseMovementController(Window &window) : window_(window) {}

  MouseMovementController::~MouseMovementController() {}

  void MouseMovementController::getCursorPos() {
    GLFWwindow *window = window_.getWindow();
    glfwGetCursorPos(window, &xpos_, &ypos_);
  }

  void MouseMovementController::getInput(GameObject &gameObject) {
    if (glfwGetMouseButton(window_.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) != 0) {
      getCursorPos();

      if (gameObject.menu
          && gameObject.menu->isInside(((xpos_ / window_.getExtent().width) - 0.5F) * 2,
                                       ((ypos_ / window_.getExtent().height) - 0.5F) * 2)) {
        gameObject.menu->setId(1);
      }
    }
  }
}  // namespace ve