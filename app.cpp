#include "app.hpp"

#include "compute_shader.hpp"
#include "interface_model.hpp"
#include "parsing.hpp"
#include "texture.hpp"

namespace ve {

  Application::Application() : m_fpscount_(0) { initDescriptor(); }

  Application::~Application() {}

  void Application::mainLoop() {
    InterfaceModel model{device_, renderer_.getSwapChainRenderPass(),
                         textureDescriptorSetLayout_->getDescriptorSetLayout()};
    Parsing parsing("lvl/lvl1.ber");

    std::vector<std::shared_ptr<Texture>> texture;
    model.loadTexture(&texture);
    model.createMenuInterface(&menuInterface_);
    model.createPlayerInterface(&playerInterface_, &menuInterface_, interfaceSize_);
    model.createGameInterface(parsing.getMap(), &gameInterface_);
    playerPointer_ = model.getPlayerPointer();
    playerCoordinate_ = model.getStartCoordinate();

    std::vector<VkDescriptorImageInfo> textureDescriptors(textureSize);
    for (int i = 0; i < textureSize; i++) {
      if (texture[i]) {
        textureDescriptors[i] = texture[i]->getImageInfo();
      }
    }

    for (int i = 0; i < static_cast<int>(textureDescriptorSets_.size()); i++) {
      DescriptorWriter(*textureDescriptorSetLayout_, *textureDescriptorPool_)
          .writeImage(0, textureDescriptors.data(), textureSize)
          .build(textureDescriptorSets_[i]);
    }

    ComputeShader computeShader{device_, renderer_.getSwapChainRenderPass(), renderer_};

    auto startTime_ = std::chrono::high_resolution_clock::now();
    auto startGameLoop = std::chrono::high_resolution_clock::now();
    startFrameTime_ = std::chrono::high_resolution_clock::now();
    double newTimeCompute;

    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();
      for (int i = 0; i < static_cast<int>(menuInterface_.size()); i++) {
        mouse_.getInput(menuInterface_[i], playerInterface_);
      }

      auto newTime = std::chrono::high_resolution_clock::now();
      size_t currentFrame = renderer_.getComputeCurrentFrame();

      switch (gameState_) {
        case GameState::START: {
          if (auto *commandBuffer = renderer_.beginFrame(false)) {
            FrameInfo frameInfo
                = {static_cast<int>(currentFrame), 0, textureDescriptorSets_[currentFrame], 0.0};

            renderer_.beginSwapChainRenderPass(commandBuffer);
            frameInfo.commandBuffer = commandBuffer;
            // start.textureRenderSystem->render(frameInfo);
            renderer_.endSwapChainRenderPass(commandBuffer);
            renderer_.endFrame(false);
          }

          if ((std::chrono::duration<float, std::chrono::seconds::period>(newTime - startTime_)
                   .count())
              >= 5) {
            newTimeCompute = glfwGetTime();
            gameState_ = GameState::PLAYING;
          }
          break;
        }

        case GameState::MENU: {
          break;
        }

        case GameState::PLAYING: {
          playerInput_.clear();
          FrameInfo frameInfo = {static_cast<int>(currentFrame), 0,
                                 textureDescriptorSets_[currentFrame], newTimeCompute};
          computeShader.render(frameInfo, menuInterface_, playerInterface_, gameInterface_);
          break;
        }

        case GameState::GAMELOOP: {
          if (playerInput_.empty()) {
            for (int i = 0; i < static_cast<int>(playerInterface_.size()); i++) {
              playerInput_.push_back(
                  std::make_pair(playerInterface_[0][i].textureRenderSystem->getIndexTexture(),
                                 playerInterface_[0][i].textureRenderSystem->getColor()));
            }
          }
          FrameInfo frameInfo = {static_cast<int>(currentFrame), 0,
                                 textureDescriptorSets_[currentFrame], newTimeCompute};
          computeShader.render(frameInfo, menuInterface_, playerInterface_, gameInterface_);

          if ((std::chrono::duration<float, std::chrono::seconds::period>(newTime - startGameLoop)
                   .count())
              >= 0.5) {
            startGameLoop = std::chrono::high_resolution_clock::now();
            gameLoop();
          }
          break;
        }

        default: {
          break;
        }
      }
      m_fpscount_++;

      updateFPS(newTime);
    }

    vkDeviceWaitIdle(device_.getDevice());
  }

  void Application::updateFPS(std::chrono::steady_clock::time_point newTime) {
    if ((std::chrono::duration<float, std::chrono::seconds::period>(newTime - startFrameTime_)
             .count())
        >= 1.0) {
      window_.updateFrame(m_fpscount_);
      m_fpscount_ = 0;
      startFrameTime_ = std::chrono::high_resolution_clock::now();
    }
  }

  void Application::initDescriptor() {
    textureDescriptorPool_ = DescriptorPool::Builder(device_)
                                 .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                              textureSize * SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .build();

    textureDescriptorSetLayout_ = DescriptorSetLayout::Builder(device_)
                                      .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                  VK_SHADER_STAGE_FRAGMENT_BIT, textureSize)
                                      .build();

    textureDescriptorSets_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  }

  void Application::gameLoop() {
    if (playerInput_[playerIndexInput_].first == TextureIndex::F0) {
      for (int i = 0; i < static_cast<int>(playerInterface_.size()); i++) {
        playerInput_.push_back(
            std::make_pair(playerInterface_[0][i].textureRenderSystem->getIndexTexture(),
                           playerInterface_[0][i].textureRenderSystem->getColor()));
      }
    } else if (playerInput_[playerIndexInput_].first == TextureIndex::F1) {
      for (int i = 0; i < static_cast<int>(playerInterface_.size()); i++) {
        playerInput_.push_back(
            std::make_pair(playerInterface_[1][i].textureRenderSystem->getIndexTexture(),
                           playerInterface_[1][i].textureRenderSystem->getColor()));
      }
    } else if (playerInput_[playerIndexInput_].first == TextureIndex::F2) {
      for (int i = 0; i < static_cast<int>(playerInterface_.size()); i++) {
        playerInput_.push_back(
            std::make_pair(playerInterface_[2][i].textureRenderSystem->getIndexTexture(),
                           playerInterface_[2][i].textureRenderSystem->getColor()));
      }
    }

    if (playerInput_[playerIndexInput_].first == TextureIndex::ARROWUP
        || playerInput_[playerIndexInput_].first == TextureIndex::ARROWRIGHT
        || playerInput_[playerIndexInput_].first == TextureIndex::ARROWLEFT) {
      for (const auto &obj : gameInterface_) {
        if (&obj != playerPointer_
            && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
          if (obj.textureRenderSystem->getColor() == playerInput_[playerIndexInput_].second
              || playerInput_[playerIndexInput_].second == glm::vec4(1.0, 1.0, 1.0, 1.0)) {
            if (playerInput_[playerIndexInput_].first == TextureIndex::ARROWUP) {
              playerPointer_->textureRenderSystem->setBuilderCoordinate(&playerCoordinate_);
            }
            if (playerInput_[playerIndexInput_].first == TextureIndex::ARROWLEFT) {
              playerCoordinate_.Angle = playerCoordinate_.Angle + 90.0F;
            }
            if (playerInput_[playerIndexInput_].first == TextureIndex::ARROWRIGHT) {
              playerCoordinate_.Angle = playerCoordinate_.Angle - 90.0F;
            }
            if (playerCoordinate_.Angle == 360.0F || playerCoordinate_.Angle == -360.0F) {
              playerCoordinate_.Angle = 0.0F;
            }
            break;
          }
        }
      }
    }

    if (playerInput_[playerIndexInput_].first == TextureIndex::BRUSHRED
        || playerInput_[playerIndexInput_].first == TextureIndex::BRUSHBLUE
        || playerInput_[playerIndexInput_].first == TextureIndex::BRUSHGREEN) {
      for (const auto &obj : gameInterface_) {
        if (&obj != playerPointer_
            && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
          if (obj.textureRenderSystem->getColor() == playerInput_[playerIndexInput_].second
              || playerInput_[playerIndexInput_].second == glm::vec4(1.0, 1.0, 1.0, 1.0)) {
            if (playerInput_[playerIndexInput_].first == TextureIndex::BRUSHRED) {
              obj.textureRenderSystem->setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
            }
            if (playerInput_[playerIndexInput_].first == TextureIndex::BRUSHGREEN) {
              obj.textureRenderSystem->setColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
            }
            if (playerInput_[playerIndexInput_].first == TextureIndex::BRUSHBLUE) {
              obj.textureRenderSystem->setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
            }
          }
        }
      }
    }

    for (const auto &obj : gameInterface_) {
      if (&obj != playerPointer_
          && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
        if (obj.textureRenderSystem->getIndexTexture() == TextureIndex::STAR) {
          obj.textureRenderSystem->setIndexTexture(TextureIndex::DISCARD);
        }
      }
    }

    if (playerIndexInput_ < static_cast<int>(playerInput_.size() - 1)) {
      playerIndexInput_++;
    }
  }

}  // namespace ve