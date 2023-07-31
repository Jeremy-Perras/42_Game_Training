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
              ((ypos_ / window_.getExtent().height) - 0.5F) * 2)) {
        index_ = menuObject.textureRenderSystem->getIndexTexture();
        color_ = menuObject.textureRenderSystem->getColor();
      }

      for (const auto &obj : playerInterface_) {
        for (const auto &playerInterface : obj) {
          if (playerInterface.textureRenderSystem
              && playerInterface.textureRenderSystem->isInside(
                  ((xpos_ / window_.getExtent().width) - 0.5F) * 2,
                  ((ypos_ / window_.getExtent().height) - 0.5F) * 2)) {
            if (index_ != TextureIndex::RED && index_ != TextureIndex::BLUE
                && index_ != TextureIndex::GREEN) {
              playerInterface.textureRenderSystem->setIndexTexture(index_);
            }
            if ((color_.x + color_.y + color_.z > 0.1)
                && playerInterface.textureRenderSystem->getIndexTexture() != TextureIndex::F0
                && playerInterface.textureRenderSystem->getIndexTexture() != TextureIndex::F1
                && playerInterface.textureRenderSystem->getIndexTexture() != TextureIndex::F2) {
              playerInterface.textureRenderSystem->setColor(color_);
            }
          }
        }
      }
    }
  }
}  // namespace ve