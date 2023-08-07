#include "mouse_movement_controller.hpp"

#include "app.hpp"

namespace ve {
  MouseMovementController::MouseMovementController(Window &window, GameState &gameState)
      : window_(window), gameState_(gameState) {}

  MouseMovementController::~MouseMovementController() {}

  void MouseMovementController::getInput(GameObject &menuInterface,
                                         std::vector<std::vector<GameObject>> &playerInterface_) {
    if (glfwGetMouseButton(window_.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      glfwGetCursorPos(window_.getGLFWwindow(), &xpos_, &ypos_);
      getUserClick(menuInterface);
      if (color_.w > 0.4F) {
        if (index_ == TextureIndex::PLAY) {
          gameState_ = GameState::GAMELOOP;
          index_ = TextureIndex::WHITE;
        }
        if (index_ == TextureIndex::STOP || index_ == TextureIndex::PAUSE) {
          gameState_ = GameState::PLAYING;
        }
        if (gameState_ == GameState::PLAYING) {
          changeUserInterface(playerInterface_);
        }
      }
    }
  }

  void MouseMovementController::getUserClick(GameObject &menuInterface) {
    if (menuInterface.textureRenderSystem
        && menuInterface.textureRenderSystem->isInside(
            ((xpos_ / window_.getExtent().width) - 0.5F) * 2,
            ((ypos_ / window_.getExtent().height) - 0.5F) * 2)
        && ((xpos_ / window_.getExtent().width) - 0.5F) * 2 < -0.6) {
      index_ = menuInterface.textureRenderSystem->getIndexTexture();
      color_ = menuInterface.textureRenderSystem->getColor();
    }
  }

  void MouseMovementController::changeUserInterface(
      std::vector<std::vector<GameObject>> &playerInterface_) {
    for (const auto &obj : playerInterface_) {
      for (const auto &playerInterface : obj) {
        if (playerInterface.textureRenderSystem
            && playerInterface.textureRenderSystem->isInside(
                ((xpos_ / window_.getExtent().width) - 0.5F) * 2,
                ((ypos_ / window_.getExtent().height) - 0.5F) * 2)) {
          if (index_ != TextureIndex::RED && index_ != TextureIndex::BLUE
              && index_ != TextureIndex::GREEN && index_ != TextureIndex::PLAY
              && index_ != TextureIndex::PAUSE && index_ != TextureIndex::STOP) {
            playerInterface.textureRenderSystem->setIndexTexture(index_);
          }
          if (color_ != glm::vec4(0.0, 0.0, 0.0, 0.0)) {
            playerInterface.textureRenderSystem->setColor(color_);
          }
        }
      }
    }
  }
}  // namespace ve