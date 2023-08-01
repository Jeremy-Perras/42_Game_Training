#include "mouse_movement_controller.hpp"

#include <iostream>

#include "game_object.hpp"
#include "model.hpp"

namespace ve {
  MouseMovementController::MouseMovementController(Window &window) : window_(window) {}

  MouseMovementController::~MouseMovementController() {}

  void MouseMovementController::getInput(GameObject &menuObject,
                                         std::vector<std::vector<GameObject>> &playerInterface_) {
    if (glfwGetMouseButton(window_.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) != 0) {
      glfwGetCursorPos(window_.getGLFWwindow(), &xpos_, &ypos_);

      if (menuObject.textureRenderSystem
          && menuObject.textureRenderSystem->isInside(
              ((xpos_ / window_.getExtent().width) - 0.5F) * 2,
              ((ypos_ / window_.getExtent().height) - 0.5F) * 2)
          && ((xpos_ / window_.getExtent().width) - 0.5F) * 2 < -0.6) {
        index_ = menuObject.textureRenderSystem->getIndexTexture();
        color_ = menuObject.textureRenderSystem->getColor();
      }
      if (!gameLoop_) {
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
      if (index_ == TextureIndex::PLAY) {
        gameLoop_ = true;
      }
      if (index_ == TextureIndex::STOP || index_ == TextureIndex::PAUSE) {
        gameLoop_ = false;
      }
    }
  }
}  // namespace ve