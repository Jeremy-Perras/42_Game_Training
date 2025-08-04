#include "app.hpp"

#include <chrono>
#include <memory>
#include <vector>

#include "device.hpp"
#include "game_loop.hpp"
#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"
#include "texture_render_system.hpp"
#include "utils.hpp"
namespace ve {

  Application::Application() : fpscount_(0), startLoadingScreen_(GameObject::createGameObject()) {
    gameLoop_ = std::make_unique<GameLoop>(device_, renderer_, gameState_, menuInterface_,
                                           playerInterface_, gameInterface_, displayInterface_,
                                           timeInterface_, menuStartInterface_, exitInterface_);

    render_system_ = std::make_unique<StarNest>(
        device_, renderer_,
        StarNest::Builder{{{{-1.0F, -1.0F}}, {{1.0F, -1.0F}}, {{1.0F, 1.0F}}, {{-1.0F, 1.0F}}},
                          {0, 1, 2, 0, 2, 3}});

    windowDisplay_ = std::make_unique<WindowDisplay>(
        device_, renderer_, menuInterface_, playerInterface_, gameInterface_, displayInterface_,
        timeInterface_, *render_system_);

    startGameLoop_ = std::chrono::high_resolution_clock::now();
    fpsTime_ = std::chrono::high_resolution_clock::now();
    passTime_ = std::chrono::high_resolution_clock::now();
    currentTime_ = std::chrono::high_resolution_clock::now();
  }

  Application::~Application() {}

  void Application::mainLoop() {
    TextureRenderSystem::Builder builder;
    builder = {{{
                    {-1, -1},

                    {0.0F, 0.0F},
                },
                {
                    {1, -1},

                    {1.0F, 0.0F},
                },
                {
                    {1, 1},

                    {1.0F, 1.0F},
                },
                {
                    {-1, 1},

                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    startLoadingScreen_.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, gameLoop_->texture_, builder, TextureIndex::BACKGROUND);

    std::chrono::steady_clock::time_point newTime = std::chrono::high_resolution_clock::now();

    KeyboardMovementController &cameraController_ = KeyboardMovementController::getInstance();
    cameraController_.setExitInterface(&exitInterface_);

    // Song test;
    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();

      for (int i = 0; i < static_cast<int>((menuInterface_).size()); i++) {
        mouse_.getInput((menuInterface_)[i], (playerInterface_));
      }

      resetTime(&currentTime_);
      float frameTime
          = std::chrono::duration<float, std::chrono::seconds::period>(currentTime_ - newTime)
                .count();
      cameraController_.moveInPlaneXY(window_.getGLFWwindow(), frameTime, gameInterface_);
      newTime = currentTime_;
      updateFrameInfo();
      logicGame();

      if ((std::chrono::duration<float, std::chrono::seconds::period>(newTime - passTime_).count())
          >= 2) {
        updateTimeScreen();
        resetTime(&passTime_);
      }
      fpscount_++;
      updateFPS(currentTime_);
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

  void Application::menuStart() {
    if (auto *commandBuffer = renderer_.beginFrame(false)) {
      renderer_.beginSwapChainRenderPass(commandBuffer);
      frameInfo_.commandBuffer = commandBuffer;
      for (auto &obj : menuStartInterface_) {
        obj.textureRenderSystem->render(frameInfo_);
      }
      renderer_.endSwapChainRenderPass(commandBuffer);
      renderer_.endFrame(false);
    }
  }

  void Application::statePlaying() {
    if (isAlreadyDone_) {
      gameLoop_->resetStatePlaying();
      gameLoop_->getPlayerInput()->clear();
      isAlreadyDone_ = false;
    }

    windowDisplay_->render(frameInfo_);
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
    windowDisplay_->render(frameInfo_);
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
    frameInfo_.frameIndex = renderer_.getComputeCurrentFrame();
    frameInfo_.commandBuffer = 0;
  }

  void Application::stateLoadingsScreen() {
    if (auto *commandBuffer = renderer_.beginFrame(false)) {
      renderer_.beginSwapChainRenderPass(commandBuffer);
      frameInfo_.commandBuffer = commandBuffer;
      startLoadingScreen_.textureRenderSystem->render(frameInfo_);
      renderer_.endSwapChainRenderPass(commandBuffer);
      renderer_.endFrame(false);
    }

    updateGameLvl();
  }

  void Application::updateGameLvl() {
    if (indexLvl < 16) {
      indexLvl++;
    }
    gameLoop_->setTexturePath(indexLvl);
    for (auto &obj : playerInterface_) {
      obj.clear();
    }
    playerInterface_.clear();
    gameInterface_.clear();
    menuInterface_.clear();
    gameLoop_->updateGameLvl();
  }

  void Application::StepByStep() {
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
    if (!gameLoop_->getPlayerInput()->empty()) {
      gameLoop_->gameLoop();
    }
    gameState_ = GameState::WAIT;
  }

  void Application::updateTimeScreen() {
    if (counterTime < static_cast<int>(timeInterface_.size())) {
      timeInterface_[counterTime++].textureRenderSystem->setIndexTexture(
          static_cast<TextureIndex>(TextureIndex::RED));
    }

    if (counterTime == static_cast<int>(timeInterface_.size())) {
      gameState_ = GameState::MENU;
      indexLvl = 0;
      counterTime = 0;
    }
  }

  void Application::logicGame() {
    switch (gameState_) {
      case GameState::STARTLOADINGSCREEN: {
        stateLoadingsScreen();
        break;
      }

      case GameState::LOADINGSCREEN: {
        break;
      }

      case GameState::EXITGAME: {
        glfwSetCharCallback(window_.getGLFWwindow(),
                            KeyboardMovementController::keyCharPressExitGame);
        glfwSetKeyCallback(window_.getGLFWwindow(), KeyboardMovementController::keyPressExitGame);
        if (glfwGetKey(window_.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS
            || exitInterface_[0].exitRenderSystem->getLogicIndex() == ExitIndex::HELLOFRIEND) {
          exitInterface_[0].exitRenderSystem->logicExitGame(cameraController_.press_, gameState_);
        }
        if (auto *commandBuffer = renderer_.beginFrame(false)) {
          renderer_.beginSwapChainRenderPass(commandBuffer);
          frameInfo_.commandBuffer = commandBuffer;
          for (auto &obj : exitInterface_) {
            obj.exitRenderSystem->render(frameInfo_);
          }
          renderer_.endSwapChainRenderPass(commandBuffer);
          renderer_.endFrame(false);
        }
        break;
      }

      case GameState::MENU: {
        menuStart();
        for (std::vector<GameObject>::iterator it = menuStartInterface_.begin();
             it != menuStartInterface_.end(); ++it) {
          if (it->textureRenderSystem->getIndexTexture() == TextureIndex::STARTBUTTON) {
            mouse_.getUserClickMenu(*it);
          }
        }
        cameraController_.moveInGameInterface(window_.getGLFWwindow(), menuStartInterface_);
        break;
      }

      case GameState::PLAYING: {
        statePlaying();
        break;
      }

      case GameState::GAMELOOP: {
        stateGameLoop(currentTime_);
        break;
      }

      case GameState::SBYS: {
        StepByStep();
        break;
      }

      case GameState::WAIT: {
        windowDisplay_->render(frameInfo_);
        break;
      }

      default: {
        break;
      }
    };
  }

}  // namespace ve