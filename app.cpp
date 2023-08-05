#include "app.hpp"

#include <memory>

#include "compute_shader.hpp"
#include "device.hpp"
#include "game_loop.hpp"
#include "interface_model.hpp"
#include "parsing.hpp"
#include "texture.hpp"
#include "texture_render_system.hpp"

namespace ve {

  Application::Application() : fpscount_(0) {}

  Application::~Application() {}

  void Application::mainLoop() {
    gameLoop_ = std::make_unique<GameLoop>(device_, renderer_, gameState_, menuInterface_,
                                           playerInterface_, gameInterface_);
    computeShader_
        = std::make_unique<ComputeShader>(device_, renderer_.getSwapChainRenderPass(), renderer_);

    firstScreenTime_ = std::chrono::high_resolution_clock::now();
    startGameLoop_ = std::chrono::high_resolution_clock::now();
    fpsTime_ = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();

    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();

      for (int i = 0; i < static_cast<int>((menuInterface_).size()); i++) {
        mouse_.getInput((menuInterface_)[i], (playerInterface_));
      }

      resetTime(&currentTime);
      updateFrameInfo();

      switch (gameState_) {
        case GameState::START: {
          stateStart(currentTime);
          break;
        }

        case GameState::MENU: {
          break;
        }

        case GameState::PLAYING: {
          statePlaying();
          break;
        }

        case GameState::GAMELOOP: {
          stateGameLoop(currentTime);
          break;
        }

        default: {
          break;
        }
      }

      fpscount_++;
      updateFPS(currentTime);
    }

    vkDeviceWaitIdle(device_.getDevice());
  }

  void Application::updateFPS(std::chrono::steady_clock::time_point currentTime) {
    if ((std::chrono::duration<float, std::chrono::seconds::period>(currentTime - fpsTime_).count())
        >= 1.0) {
      window_.updateFrame(fpscount_);
      fpscount_ = 0;
      resetTime(&fpsTime_);
    }
  }

  void Application::stateStart(std::chrono::steady_clock::time_point currentTime) {
    if (auto *commandBuffer = renderer_.beginFrame(false)) {
      renderer_.beginSwapChainRenderPass(commandBuffer);
      frameInfo_.commandBuffer = commandBuffer;
      // start.textureRenderSystem->render(frameInfo);
      renderer_.endSwapChainRenderPass(commandBuffer);
      renderer_.endFrame(false);
    }

    if ((std::chrono::duration<float, std::chrono::seconds::period>(currentTime - firstScreenTime_)
             .count())
        >= 5) {
      frameInfo_.Time = glfwGetTime();
      gameState_ = GameState::PLAYING;
    }
  }

  void Application::statePlaying() {
    if (isAlreadyDone_) {
      gameLoop_->resetStatePlaying();
      gameLoop_->getPlayerInput()->clear();
      isAlreadyDone_ = false;
    }
    computeShader_->render(frameInfo_, menuInterface_, playerInterface_, gameInterface_);
  }

  void Application::stateGameLoop(std::chrono::steady_clock::time_point currentTime) {
    if (gameLoop_->getPlayerInput()->empty() && !isAlreadyDone_) {
      for (int i = 0; i < static_cast<int>(playerInterface_[0].size()); i++) {
        if (!GameLoop::isTextureWhite(
                playerInterface_[0][i].textureRenderSystem->getIndexTexture())) {
          gameLoop_->getPlayerInput()->push_back(
              std::make_pair(playerInterface_[0][i].textureRenderSystem->getIndexTexture(),
                             playerInterface_[0][i].textureRenderSystem->getColor()));
        }
      }
      isAlreadyDone_ = true;
    }
    gameLoop_->playerDead();
    computeShader_->render(frameInfo_, menuInterface_, playerInterface_, gameInterface_);
    if ((std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startGameLoop_)
             .count())
        >= timeUpdateGame_) {
      resetTime(&startGameLoop_);
      if (!gameLoop_->getPlayerInput()->empty()) {
        gameLoop_->gameLoop();
      }
    }
  }

  void Application::resetTime(std::chrono::steady_clock::time_point *time) {
    *time = std::chrono::high_resolution_clock::now();
  }

  void Application::updateFrameInfo() {
    size_t currentFrame = renderer_.getComputeCurrentFrame();
    frameInfo_
        = {renderer_.getComputeCurrentFrame(), 0, gameLoop_->getDescriptorSets(currentFrame), 0.0F};
  }

}  // namespace ve