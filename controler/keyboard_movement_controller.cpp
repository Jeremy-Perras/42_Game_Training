#include "keyboard_movement_controller.hpp"

#include <_types/_uint32_t.h>

// std
#include <iostream>
#include <vector>

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
    int i = 4;
    static int oldStateUp = GLFW_RELEASE;
    static int oldStateDown = GLFW_RELEASE;

    int stateDown = glfwGetKey(window, keys_.moveDown);
    int stateUp = glfwGetKey(window, keys_.moveUp);

    if (stateDown == GLFW_RELEASE && oldStateDown == GLFW_PRESS) {
      while (menuInterface[i].textureRenderSystem->getIndexTexture() == TextureIndex::DONOTSHOW
             && i < 7) {
        i++;
      }
      if (i >= 4 && i < 7) {
        menuInterface[i].textureRenderSystem->setIndexTexture(TextureIndex::DONOTSHOW);
        menuInterface[i + 1].textureRenderSystem->setIndexTexture(TextureIndex::PLAYERRIGHT);
      }
    }
    oldStateDown = stateDown;

    if (stateUp == GLFW_RELEASE && oldStateUp == GLFW_PRESS) {
      while (menuInterface[i].textureRenderSystem->getIndexTexture() == TextureIndex::DONOTSHOW
             && i < 7) {
        i++;
      }
      if (i > 4 && i <= 7) {
        menuInterface[i - 1].textureRenderSystem->setIndexTexture(TextureIndex::PLAYERRIGHT);
        menuInterface[i].textureRenderSystem->setIndexTexture(TextureIndex::DONOTSHOW);
      }
    }
    oldStateUp = stateUp;
  }

  void KeyboardMovementController::getKeyPressMenu(GLFWwindow* window, GameState& gameState,
                                                   std::vector<GameObject>& menuInterface) {
    int enter = glfwGetKey(window, GLFW_KEY_ENTER);
    if (enter != 0
        && menuInterface[4].textureRenderSystem->getIndexTexture() == TextureIndex::PLAYERRIGHT) {
      gameState = GameState::PLAYING;
    }
  }
}  // namespace ve
