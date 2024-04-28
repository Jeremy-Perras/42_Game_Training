#include "keyboard_movement_controller.hpp"

#include <_types/_uint32_t.h>

// std
#include <limits>
#include <vector>

#include "game_object.hpp"

namespace ve {
  void KeyboardMovementController::moveInPlaneXY(GLFWwindow* window, float dt,
                                                 std::vector<GameObject>& gameInterface) const {
    if (glfwGetKey(window, keys_.moveUp) == GLFW_PRESS) {
      for (auto& obj : gameInterface) {
        obj.textureRenderSystem->setPushTranslationCoordinate(-moveSpeed_ * dt, false, true);
      }
    }
    if (glfwGetKey(window, keys_.moveDown) == GLFW_PRESS) {
      for (auto& obj : gameInterface) {
        obj.textureRenderSystem->setPushTranslationCoordinate(moveSpeed_ * dt, false, true);
      }
    }
    if (glfwGetKey(window, keys_.moveRight) == GLFW_PRESS) {
      for (auto& obj : gameInterface) {
        obj.textureRenderSystem->setPushTranslationCoordinate(moveSpeed_ * dt, true, false);
      }
    }
    if (glfwGetKey(window, keys_.moveLeft) == GLFW_PRESS) {
      for (auto& obj : gameInterface) {
        obj.textureRenderSystem->setPushTranslationCoordinate(-moveSpeed_ * dt, true, false);
      }
    }
  }

  void KeyboardMovementController::moveInGameInterface(
      GLFWwindow* window, std::vector<GameObject>& menuInterface) const {
    if (glfwGetKey(window, keys_.moveDown) == GLFW_PRESS) {
      menuInterface[1].textureRenderSystem->setIndexTexture(TextureIndex::DONOTSHOW);
      menuInterface[2].textureRenderSystem->setIndexTexture(TextureIndex::PLAYERRIGHT);
    }
    if (glfwGetKey(window, keys_.moveUp) == GLFW_PRESS) {
      menuInterface[1].textureRenderSystem->setIndexTexture(TextureIndex::PLAYERRIGHT);
      menuInterface[2].textureRenderSystem->setIndexTexture(TextureIndex::DONOTSHOW);
    }
  }
}  // namespace ve