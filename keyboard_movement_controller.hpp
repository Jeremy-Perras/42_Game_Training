#pragma once

#include "game_object.hpp"

namespace ve {
  class KeyboardMovementController {
  public:
    struct KeyMappings {
      int moveLeft = GLFW_KEY_A;
      int moveRight = GLFW_KEY_D;
      int moveUp = GLFW_KEY_W;
      int moveDown = GLFW_KEY_S;
    };
  };
}  // namespace ve