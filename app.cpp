#include "app.hpp"

#include <unistd.h>

#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "buffer.hpp"
#include "compute_shader.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"
#include "model.hpp"
#include "parsing.hpp"
#include "render_system.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"
#include "texture_render_system.hpp"

namespace ve {

  Application::Application() : m_fpscount_(0) { initDescriptor(); }

  Application::~Application() {}

  void Application::mainLoop() {
    Model model{device_, renderer_.getSwapChainRenderPass(),
                descriptorSetLayout_->getDescriptorSetLayout()};
    Parsing parsing("lvl/lvl1.ber", device_, renderer_.getSwapChainRenderPass(),
                    descriptorSetLayout_->getDescriptorSetLayout());

    interfaceSize interfaceSize{3, 3, 4};

    std::vector<std::shared_ptr<Texture>> texture;
    model.loadTexture(&texture);
    model.menuInterface(&gameObjects_);
    model.playerInterface(&playerInterface_, &gameObjects_, interfaceSize);
    parsing.createMap(&gameInterface_);
    playerPointer_ = parsing.getPlayerPointer();
    playerCoordinate_ = parsing.getStartCoordinate();

    std::vector<VkDescriptorImageInfo> textureDescriptors(textureSize);
    for (int i = 0; i < textureSize; i++) {
      if (texture[i]) {
        textureDescriptors[i] = texture[i]->getImageInfo();
      }
    }

    for (int i = 0; i < static_cast<int>(descriptorSets_.size()); i++) {
      auto bufferInfo = uboBuffers_[i]->descriptorInfo();
      DescriptorWriter(*descriptorSetLayout_, *globalPool_)
          .writeBuffer(0, &bufferInfo)
          .writeImage(1, textureDescriptors.data(), textureSize)
          .build(descriptorSets_[i]);
    }

    ComputeShader computeShader{device_, renderer_.getSwapChainRenderPass(), renderer_};

    auto startTime_ = std::chrono::high_resolution_clock::now();
    auto startGameLoop = std::chrono::high_resolution_clock::now();
    startFrameTime_ = std::chrono::high_resolution_clock::now();
    double newTimeCompute;

    // std::vector<TextureRenderSystem::Builder> builder;
    // builder.push_back({{{{-1, -1},

    //                      {0.0F, 0.0F},
    //                      {1.0, 1.0, 1.0, 1.0}},
    //                     {{1, -1},

    //                      {1.0F, 0.0F},
    //                      {1.0, 1.0, 1.0, 1.0}},
    //                     {{1, 1},

    //                      {1.0F, 1.0F},
    //                      {1.0, 1.0, 1.0, 1.0}},
    //                     {{-1, 1},

    //                      {0.0F, 1.0F},
    //                      {1.0, 1.0, 1.0, 1.0}}},
    //                    {0, 1, 2, 2, 3, 0}});
    // auto start = GameObject::createGameObject();
    // start.textureRenderSystem = std::make_unique<TextureRenderSystem>(
    //     device_, renderer_.getSwapChainRenderPass(),
    //     descriptorSetLayout_->getDescriptorSetLayout(), builder[0], TextureIndex::BACKGROUND);

    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();
      for (int i = 0; i < static_cast<int>(gameObjects_.size() - 1); i++) {
        mouse_.getInput(gameObjects_[i], playerInterface_);
      }
      if (mouse_.getGameLoop()) {
        gameState_ = GameState::GAMELOOP;
      } else if (gameState_ != GameState::PLAYING) {
        parsing.createMap(&gameInterface_);
        playerPointer_ = parsing.getPlayerPointer();
        playerCoordinate_ = parsing.getStartCoordinate();
        gameState_ = GameState::PLAYING;
      }
      GlobalUbo ubo{};
      auto newTime = std::chrono::high_resolution_clock::now();
      ubo.deltaTime
          = std::chrono::duration<float, std::chrono::seconds::period>(newTime - startTime_)
                .count();
      size_t currentFrame = renderer_.getComputeCurrentFrame();

      m_fpscount_++;

      switch (gameState_) {
        case GameState::START: {
          if (auto *commandBuffer = renderer_.beginFrame(false)) {
            FrameInfo frameInfo
                = {static_cast<int>(currentFrame), 0, descriptorSets_[currentFrame], 0.0};

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
          FrameInfo frameInfo
              = {static_cast<int>(currentFrame), 0, descriptorSets_[currentFrame], newTimeCompute};
          computeShader.render(frameInfo, gameObjects_, playerInterface_, gameInterface_);
          break;
        }

        case GameState::GAMELOOP: {
          // for (const auto &obj : gameInterface_){

          // }
          FrameInfo frameInfo
              = {static_cast<int>(currentFrame), 0, descriptorSets_[currentFrame], newTimeCompute};
          computeShader.render(frameInfo, gameObjects_, playerInterface_, gameInterface_);

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
    globalPool_
        = DescriptorPool::Builder(device_)
              .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           textureSize * SwapChain::MAX_FRAMES_IN_FLIGHT)
              .build();
    uboBuffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < static_cast<int>(uboBuffers_.size()); i++) {
      uboBuffers_[i] = std::make_unique<Buffer>(device_, sizeof(GlobalUbo), 1,
                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      uboBuffers_[i]->map();
    }

    descriptorSetLayout_
        = DescriptorSetLayout::Builder(device_)
              .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
              .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          VK_SHADER_STAGE_FRAGMENT_BIT, textureSize)
              .build();

    descriptorSets_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  }

  void Application::gameLoop() {
    TextureIndex textureIndex
        = playerInterface_[playerStatue_.ligne][playerStatue_.colone[playerStatue_.ligne]]
              .textureRenderSystem->getIndexTexture();
    glm::vec4 color
        = playerInterface_[playerStatue_.ligne][playerStatue_.colone[playerStatue_.ligne]]
              .textureRenderSystem->getColor();

    if (textureIndex == TextureIndex::F0) {
      playerStatue_.incremetation(static_cast<int>(playerInterface_[0].size()) - 1,
                                  playerStatue_.ligne);
      playerStatue_.ligne = 0;
    } else if (textureIndex == TextureIndex::F1) {
      playerStatue_.incremetation(static_cast<int>(playerInterface_[1].size()) - 1,
                                  playerStatue_.ligne);
      playerStatue_.ligne = 1;
    } else if (textureIndex == TextureIndex::F2) {
      playerStatue_.incremetation(static_cast<int>(playerInterface_[2].size()) - 1,
                                  playerStatue_.ligne);
      playerStatue_.ligne = 2;
    }
    if (textureIndex == TextureIndex::ARROWUP || textureIndex == TextureIndex::ARROWRIGHT
        || textureIndex == TextureIndex::ARROWLEFT) {
      for (const auto &obj : gameInterface_) {
        if (&obj != playerPointer_
            && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
          if (obj.textureRenderSystem->getColor() == color
              || color == glm::vec4(1.0, 1.0, 1.0, 1.0)) {
            std::cout << "test" << std::endl;
            if (textureIndex == TextureIndex::ARROWUP) {
              playerPointer_->textureRenderSystem->setBuilderCoordinate(&playerCoordinate_);
            }
            if (textureIndex == TextureIndex::ARROWLEFT) {
              playerCoordinate_.Angle = playerCoordinate_.Angle + 90.0F;
            }
            if (textureIndex == TextureIndex::ARROWRIGHT) {
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

    if (textureIndex == TextureIndex::BRUSHRED || textureIndex == TextureIndex::BRUSHBLUE
        || textureIndex == TextureIndex::BRUSHGREEN) {
      for (const auto &obj : gameInterface_) {
        if (&obj != playerPointer_
            && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
          if (obj.textureRenderSystem->getColor() == color
              || color == glm::vec4(1.0, 1.0, 1.0, 1.0)) {
            if (textureIndex == TextureIndex::BRUSHRED) {
              obj.textureRenderSystem->setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
            }
            if (textureIndex == TextureIndex::BRUSHBLUE) {
              obj.textureRenderSystem->setColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
            }
            if (textureIndex == TextureIndex::BRUSHGREEN) {
              obj.textureRenderSystem->setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
            }
          }
        }
      }
    }
    playerStatue_.incremetation(static_cast<int>(playerInterface_[playerStatue_.ligne].size()) - 1,
                                playerStatue_.ligne);
  }

}  // namespace ve