#include "keyboard_movement_controller.hpp"

#include <_types/_uint32_t.h>

// std
#include <iostream>
#include <vector>

#include "game_object.hpp"
#include "utils.hpp"
namespace ve {

  void KeyboardMovementController::keyCharPressExitGame(GLFWwindow* /*window*/,
                                                        unsigned int codepoint) {
    getInstance().getKeyPressCharExitGame(codepoint);
  }

  void KeyboardMovementController::keyPressExitGame(GLFWwindow* /*window*/, int key,
                                                    int /*scancode*/, int action, int /*mods*/) {
    getInstance().getKeyPressExitGame(key, action);
  }

  void KeyboardMovementController::getKeyPressCharExitGame(unsigned int codepoint) {
    if (((codepoint < 123 && codepoint > 96) || (codepoint < 91 && codepoint > 64)
         || codepoint == 32)) {
      press_.push_back(static_cast<char>(codepoint));
    }
    std::cout << press_ << '\n';
  }

  void KeyboardMovementController::getKeyPressExitGame(int key, int action) {
    exitInterface_->at(0).exitRenderSystem->homeState(press_);
    if (action == GLFW_PRESS && key == 259 && !press_.empty()) {
      press_.pop_back();
    }
  }

  void KeyboardMovementController::deleteKeyPress() {
    if (!press_.empty()) {
      press_.clear();
    }
  }

  KeyboardMovementController& KeyboardMovementController::getInstance() {
    static KeyboardMovementController instance;
    return instance;
  }

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
      menuInterface[2].textureRenderSystem->setIndexTexture(TextureIndex::DONOTSHOW);
      menuInterface[3].textureRenderSystem->setIndexTexture(TextureIndex::PLAYERRIGHT);
    }
    if (glfwGetKey(window, keys_.moveUp) == GLFW_PRESS) {
      menuInterface[2].textureRenderSystem->setIndexTexture(TextureIndex::PLAYERRIGHT);
      menuInterface[3].textureRenderSystem->setIndexTexture(TextureIndex::DONOTSHOW);
    }
  }

}  // namespace ve
