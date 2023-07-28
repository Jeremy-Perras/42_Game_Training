#include "app.hpp"

#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

#include "buffer.hpp"
#include "compute_shader.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"
#include "menu.hpp"
#include "model.hpp"
#include "simple_render_system.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"
namespace ve {

  Application::Application() : m_fpscount_(0) {
    initDescriptor();
    createObject();
  }

  Application::~Application() {}

  void Application::mainLoop() {
    Texture texture(device_, "texture/test2.png");
    Texture texture2(device_, "texture/texture.jpg");
    auto test = std::make_unique<Texture>(device_, "texture/test2.png");
    auto test2 = std::make_unique<Texture>(device_, "texture/test2.png");
    std::vector<std::shared_ptr<Texture>> texture3;
    texture3.push_back(std::move(test2));
    texture3.push_back(std::move(test));

    for (int i = 0; i < static_cast<int>(descriptorSets_.size()); i++) {
      auto bufferInfo = uboBuffers_[i]->descriptorInfo();
      auto textureInfo = texture3[0]->getImageInfo();
      auto textureInfo2 = texture3[1]->getImageInfo();
      DescriptorWriter(*descriptorSetLayout_, *globalPool_)
          .writeBuffer(0, &bufferInfo)
          .writeImage(1, &textureInfo)
          .writeImage(2, &textureInfo2)
          .build(descriptorSets_[i]);
    }

    ComputeShader::Builder computeBuilder{};
    ComputeShader computeShader{device_, renderer_.getSwapChainRenderPass(), renderer_,
                                computeBuilder};

    auto startTime_ = std::chrono::high_resolution_clock::now();
    startFrameTime_ = std::chrono::high_resolution_clock::now();
    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();
      for (int i = 0; i < static_cast<int>(gameObjects_.size()); i++) {
        mouse_.getInput(gameObjects_[i]);
      }
      GlobalUbo ubo{};
      auto newTime = std::chrono::high_resolution_clock::now();
      ubo.deltaTime
          = std::chrono::duration<float, std::chrono::seconds::period>(newTime - startTime_)
                .count();
      size_t currentFrame = renderer_.getComputeCurrentFrame();
      m_fpscount_++;

      FrameInfo frameInfo = {static_cast<int>(currentFrame), 0, descriptorSets_[currentFrame]};
      computeShader.render(frameInfo, gameObjects_);
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
                           SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           SwapChain::MAX_FRAMES_IN_FLIGHT)
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
                          VK_SHADER_STAGE_FRAGMENT_BIT)
              .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          VK_SHADER_STAGE_FRAGMENT_BIT)
              .build();

    descriptorSets_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  }

  void Application::createObject() {
    float xStart = -0.95;
    float yStart = 0.80F;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        builder_.push_back(
            {{{{xStart + static_cast<float>(i) * 0.05F, yStart + static_cast<float>(j) * 0.05},
               {1.0F, 0.0F, 0.0F},
               {1.0F, 0.0F}},
              {{xStart + 0.05F + static_cast<float>(i) * 0.05F,
                yStart + static_cast<float>(j) * 0.05},
               {1.0F, 0.0F, 0.0F},
               {0.0F, 0.0F}},
              {{xStart + 0.05F + static_cast<float>(i) * 0.05F,
                yStart + 0.05F + static_cast<float>(j) * 0.05},
               {1.0F, 0.0F, 0.0F},
               {0.0F, 1.0F}},
              {{xStart + static_cast<float>(i) * 0.05F,
                yStart + 0.05F + static_cast<float>(j) * 0.05},
               {1.0F, 0.0F, 0.0F},
               {1.0F, 1.0F}}},
             {0, 1, 2, 2, 3, 0}});
      }
    }
    for (int j = 0; j < 3; j++) {
      builder_.push_back(
          {{{{xStart + 0.01F + 3 * 0.05F, yStart + static_cast<float>(j) * 0.05},
             {1.0F, 0.0F, 0.0F},
             {1.0F, 0.0F}},
            {{xStart + 0.05F + 0.01F + 3 * 0.05F, yStart + static_cast<float>(j) * 0.05},
             {1.0F, 0.0F, 0.0F},
             {0.0F, 0.0F}},
            {{xStart + 0.05F + 0.01F + 3 * 0.05F, yStart + 0.05F + static_cast<float>(j) * 0.05},
             {1.0F, 0.0F, 0.0F},
             {0.0F, 1.0F}},
            {{xStart + 0.01F + 3 * 0.05F, yStart + 0.05F + static_cast<float>(j) * 0.05},
             {1.0F, 0.0F, 0.0F},
             {1.0F, 1.0F}}},
           {0, 1, 2, 2, 3, 0}});
    }
    auto squareMenuTop = GameObject::createGameObject();
    auto squareMenuTop2 = GameObject::createGameObject();
    auto squareMenuTop3 = GameObject::createGameObject();
    auto squareMenuTop4 = GameObject::createGameObject();
    squareMenuTop.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[0], 0);
    squareMenuTop2.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[1], 0);
    squareMenuTop3.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[2], 0);
    squareMenuTop4.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[9], 0);
    gameObjects_.push_back(std::move(squareMenuTop));
    gameObjects_.push_back(std::move(squareMenuTop2));
    gameObjects_.push_back(std::move(squareMenuTop3));
    gameObjects_.push_back(std::move(squareMenuTop4));

    auto squareMenuMiddle = GameObject::createGameObject();
    auto squareMenuMiddle2 = GameObject::createGameObject();
    auto squareMenuMiddle3 = GameObject::createGameObject();
    auto squareMenuMiddle4 = GameObject::createGameObject();
    squareMenuMiddle.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[3], 0);
    squareMenuMiddle2.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[4], 0);
    squareMenuMiddle3.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[5], 0);
    squareMenuMiddle4.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[10], 0);
    gameObjects_.push_back(std::move(squareMenuMiddle));
    gameObjects_.push_back(std::move(squareMenuMiddle2));
    gameObjects_.push_back(std::move(squareMenuMiddle3));
    gameObjects_.push_back(std::move(squareMenuMiddle4));

    auto squareMenuBack = GameObject::createGameObject();
    auto squareMenuBack2 = GameObject::createGameObject();
    auto squareMenuBack3 = GameObject::createGameObject();
    auto squareMenuBack4 = GameObject::createGameObject();
    squareMenuBack.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[6], 0);
    squareMenuBack2.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[7], 1);
    squareMenuBack3.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[8], 1);
    squareMenuBack4.menu = std::make_unique<MenuSystem>(
        device_, renderer_.getSwapChainRenderPass(), descriptorSetLayout_->getDescriptorSetLayout(),
        builder_[11], 1);
    gameObjects_.push_back(std::move(squareMenuBack));
    gameObjects_.push_back(std::move(squareMenuBack2));
    gameObjects_.push_back(std::move(squareMenuBack3));
    gameObjects_.push_back(std::move(squareMenuBack4));
    auto whiteSquare = GameObject::createGameObject();
    whiteSquare.model = Model::createSquareModel(device_);
    gameObjects_.push_back(std::move(whiteSquare));
  }

}  // namespace ve