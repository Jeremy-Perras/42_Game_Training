#include "app.hpp"

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
#include "render_system.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"

namespace ve {

  Application::Application() : m_fpscount_(0) { initDescriptor(); }

  Application::~Application() {}

  void Application::mainLoop() {
    Model model{device_};
    std::vector<std::shared_ptr<Texture>> texture;
    model.loadTexture(&texture);
    model.createMenu(&gameObjects_, renderer_.getSwapChainRenderPass(),
                     descriptorSetLayout_->getDescriptorSetLayout());
    std::vector<VkDescriptorImageInfo> textureDescriptors(textureSize);
    for (int i = 0; i < textureSize; i++) {
      textureDescriptors[i] = texture[i]->getImageInfo();
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
    startFrameTime_ = std::chrono::high_resolution_clock::now();
    double newTimeCompute;

    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();
      for (int i = 0; i < static_cast<int>(gameObjects_.size() - 1); i++) {
        mouse_.getInput(gameObjects_[i]);
      }
      GlobalUbo ubo{};
      auto newTime = std::chrono::high_resolution_clock::now();
      ubo.deltaTime
          = std::chrono::duration<float, std::chrono::seconds::period>(newTime - startTime_)
                .count();
      size_t currentFrame = renderer_.getComputeCurrentFrame();

      m_fpscount_++;

      switch (gameState_) {
        case GameState::START:

          if (auto *commandBuffer = renderer_.beginFrame(false)) {
            FrameInfo frameInfo
                = {static_cast<int>(currentFrame), 0, descriptorSets_[currentFrame], 0.0};

            renderer_.beginSwapChainRenderPass(commandBuffer);
            frameInfo.commandBuffer = commandBuffer;
            gameObjects_[gameObjects_.size() - 2].renderSystem->renderGameObjects(frameInfo);
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
        case GameState::MENU:
          break;
        case GameState::PLAYING:

          FrameInfo frameInfo
              = {static_cast<int>(currentFrame), 0, descriptorSets_[currentFrame], newTimeCompute};
          computeShader.render(frameInfo, gameObjects_);
          break;
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

}  // namespace ve