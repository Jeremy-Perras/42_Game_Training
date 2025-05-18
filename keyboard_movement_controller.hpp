#pragma once

#include <string>

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
    void moveInPlaneXY(GLFWwindow* window, float dt, std::vector<GameObject>& gameInterface) const;
    void moveInGameInterface(GLFWwindow* window, std::vector<GameObject>& menuInterface) const;
    void static keyCharPressExitGame(GLFWwindow* /*window*/, unsigned int codepoint);
    void static keyPressExitGame(GLFWwindow* window, int key, int scancode, int action, int mods);
    static KeyboardMovementController& getInstance();
    void getKeyPressCharExitGame(unsigned int codepoint);
    void setExitGameInterface();
    void getKeyPressExitGame(int key, int action);
    void setExitInterface(std::vector<GameObject>* exitInterface) {
      exitInterface_ = exitInterface;
    }
    void deleteKeyPress();

    KeyMappings keys_{};
    float moveSpeed_{0.5F};
    std::string press_{};

  private:
    std::vector<GameObject>* exitInterface_;

    KeyboardMovementController() {};
    KeyboardMovementController(const KeyboardMovementController& src) = delete;
    KeyboardMovementController& operator=(const KeyboardMovementController& rhs) = delete;
    ~KeyboardMovementController() {};
  };
}  // namespace ve