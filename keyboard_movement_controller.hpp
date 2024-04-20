#pragma once

#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"

namespace ve {
  class KeyboardMovementController {
  public:
    struct KeyMappings {
      int moveLeft = GLFW_KEY_A;
      int moveRight = GLFW_KEY_D;
      int moveUp = GLFW_KEY_W;
      int moveDown = GLFW_KEY_S;
    };
    void moveInPlaneXY(GLFWwindow* window, float dt, std::vector<GameObject>& gameInterface) const;
    void moveInGameInterface(GLFWwindow* window, std::vector<GameObject>& menuInterface) const;
    KeyMappings keys_{};
    float moveSpeed_{0.5F};
  };
}  // namespace ve