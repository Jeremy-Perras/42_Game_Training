#include "keyboard_movement_controller.hpp"

#include <_types/_uint32_t.h>

// std
#include <limits>
#include <vector>

#include "game_object.hpp"

namespace ve {
  void KeyboardMovementController::moveInPlaneXY(GLFWwindow* window, float dt,
                                                 std::vector<GameObject>& gameInterface) const {
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) {
      for (auto& obj : gameInterface) {
        obj.textureRenderSystem->setPushTranslationCoordinate(-moveSpeed * dt, false, true);
      }
    }
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) {
      for (auto& obj : gameInterface) {
        obj.textureRenderSystem->setPushTranslationCoordinate(moveSpeed * dt, false, true);
      }
    }
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) {
      for (auto& obj : gameInterface) {
        obj.textureRenderSystem->setPushTranslationCoordinate(moveSpeed * dt, true, false);
      }
    }
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) {
      for (auto& obj : gameInterface) {
        obj.textureRenderSystem->setPushTranslationCoordinate(-moveSpeed * dt, true, false);
      }
    }
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) {
      for (auto& obj : gameInterface) {
        obj.textureRenderSystem->setPushTranslationCoordinate(-moveSpeed * dt, true, false);
      }
    }
  }
}  // namespace ve