#include "mouse_movement_controller.hpp"

#include <iostream>
#include <vector>

#include "game_object.hpp"
#include "utils.hpp"

namespace ve {
  MouseMovementController::MouseMovementController(Window &window, GameState &gameState)
      : window_(window), gameState_(gameState) {}

  MouseMovementController::~MouseMovementController() {}

  void MouseMovementController::setInput() {
    glfwGetCursorPos(window_.getGLFWwindow(), &xpos_, &ypos_);
  }

  void MouseMovementController::getInput(GameObject &menuInterface,
                                         std::vector<std::vector<GameObject>> &playerInterface_) {
    if (glfwGetMouseButton(window_.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      mouseSet_ = true;
      coordinatesMouse();
      getUserClick(menuInterface);
      if (color_.w > 0.4F) {
        if (index_ == TextureIndex::PLAY) {
          gameState_ = GameState::GAMELOOP;
          index_ = TextureIndex::WHITE;
        } else if (index_ == TextureIndex::STOP) {
          gameState_ = GameState::PLAYING;
        } else if (index_ == TextureIndex::STEPBYSTEP) {
          std::cout << "step by step" << std::endl;
          gameState_ = GameState::SBYS;
          index_ = TextureIndex::WHITE;
        } else if (gameState_ == GameState::PLAYING) {
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
              && index_ != TextureIndex::STOP) {
            playerInterface.textureRenderSystem->setIndexTexture(index_);
          }
          if (color_ != glm::vec4(0.0, 0.0, 0.0, 0.0) && index_ != TextureIndex::PLAY
              && index_ != TextureIndex::STOP) {
            playerInterface.textureRenderSystem->setColor(color_);
          }
        }
      }
    }
  }

  void MouseMovementController::getUserClickMenu(GameObject &menuStartInterface) {
    coordinatesMouse();
    if (menuStartInterface.textureRenderSystem
        && menuStartInterface.textureRenderSystem->isInside(xposWindow_, yposWindow_)) {
      menuStartInterface.textureRenderSystem->setColor(glm::vec4(0.7, 0.04, 0.0, 1));
      glfwGetMouseButton(window_.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS
          ? gameState_ = GameState::STARTLOADINGSCREEN
          : gameState_ = GameState::MENU;
    } else {
      menuStartInterface.textureRenderSystem->setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
    }
  }

  void MouseMovementController::coordinatesMouse() {
    glfwGetCursorPos(window_.getGLFWwindow(), &xpos_, &ypos_);
    xposWindow_ = ((xpos_ / window_.getExtent().width) - 0.5F) * 2;   // -1.0F to 1.0F;
    yposWindow_ = ((ypos_ / window_.getExtent().height) - 0.5F) * 2;  // -1.0F to 1.0F;
  }

}  // namespace ve