#include "app.hpp"

#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <ostream>
#include <utility>

#include "buffer.hpp"
#include "compute_shader.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"
#include "menu.hpp"
#include "model.hpp"
#include "simple_render_system.hpp"
#include "texture.hpp"
namespace ve {
  struct GlobalUbo {
    float deltaTime;
  };

  Application::Application() : m_fpscount_(0) {
    globalPool_
        = DescriptorPool::Builder(device_)
              .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           SwapChain::MAX_FRAMES_IN_FLIGHT)
              .build();

    globalPool2_
        = DescriptorPool::Builder(device_)
              .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           SwapChain::MAX_FRAMES_IN_FLIGHT)
              .build();
    loadGameObjects();
  }

  Application::~Application() {}

  void Application::mainLoop() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < static_cast<int>(uboBuffers.size()); i++) {
      uboBuffers[i] = std::make_unique<Buffer>(device_, sizeof(GlobalUbo), 1,
                                               VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      uboBuffers[i]->map();
    }

    auto globalSetLayout
        = DescriptorSetLayout::Builder(device_)
              .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
              .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          VK_SHADER_STAGE_FRAGMENT_BIT)
              .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          VK_SHADER_STAGE_FRAGMENT_BIT)
              .build();

    auto globalSetLayout2
        = DescriptorSetLayout::Builder(device_)
              .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
              .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          VK_SHADER_STAGE_FRAGMENT_BIT)
              .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          VK_SHADER_STAGE_FRAGMENT_BIT)
              .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    std::vector<VkDescriptorSet> globalDescriptorSets2(SwapChain::MAX_FRAMES_IN_FLIGHT);

    Texture texture(device_, "texture/test2.png");
    Texture texture2(device_, "texture/texture.jpg");

    for (int i = 0; i < static_cast<int>(globalDescriptorSets.size()); i++) {
      auto bufferInfo = uboBuffers[i]->descriptorInfo();
      auto textureInfo = texture.getImageInfo();
      auto textureInfo2 = texture2.getImageInfo();
      DescriptorWriter(*globalSetLayout, *globalPool_)
          .writeBuffer(0, &bufferInfo)
          .writeImage(1, &textureInfo)
          .writeImage(2, &textureInfo2)
          .build(globalDescriptorSets[i]);
    }

    for (int i = 0; i < static_cast<int>(globalDescriptorSets.size()); i++) {
      auto bufferInfo = uboBuffers[i]->descriptorInfo();
      auto textureInfo = texture.getImageInfo();
      auto textureInfo2 = texture2.getImageInfo();
      DescriptorWriter(*globalSetLayout2, *globalPool2_)
          .writeBuffer(0, &bufferInfo)
          .writeImage(1, &textureInfo)
          .writeImage(2, &textureInfo2)
          .build(globalDescriptorSets2[i]);
    }

    MenuSystem::Builder builder{{{{-1.00F, -1.00F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F}},
                                 {{-0.90F, -1.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
                                 {{-0.90F, -0.90F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F}},
                                 {{-1.00F, -0.90F}, {1.0F, 0.0F, 0.0F}, {1.0F, 1.0F}}},
                                {0, 1, 2, 2, 3, 0}};

    MenuSystem menuSystem{device_, renderer_.getSwapChainRenderPass(),
                          globalSetLayout->getDescriptorSetLayout(), builder, 0};

    MenuSystem::Builder builder2{{{{-0.05F, -0.05F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F}},
                                  {{0.05F, -0.05F}, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F}},
                                  {{0.05F, 0.05F}, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F}},
                                  {{-0.05F, 0.05F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F}}},
                                 {0, 1, 2, 2, 3, 0}};

    MenuSystem menuSystem2{device_, renderer_.getSwapChainRenderPass(),
                           globalSetLayout2->getDescriptorSetLayout(), builder2, 1};

    SimpleRenderSystem simpleRenderSystem{device_, renderer_.getSwapChainRenderPass()};
    // KeyboardMovementController playerController{};
    //                             computeBuilder);
    ComputeShader::Builder computeBuilder{};
    ComputeShader computeShader{device_, renderer_.getSwapChainRenderPass(), renderer_,
                                computeBuilder};

    auto startTime = std::chrono::high_resolution_clock::now();
    auto startFrameTime = std::chrono::high_resolution_clock::now();
    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();
      m_fpscount_++;
      auto newTime = std::chrono::high_resolution_clock::now();
      FrameInfo frameInfo{0, 0, 0};

      // playerController.moveInPlaneXZ(window_.getGLFWwindow(), gameObjects_[0]);
      //
      if (auto* commandBuffer = renderer_.beginFrame(false)) {
        GlobalUbo ubo{};
        ubo.deltaTime
            = std::chrono::duration<float, std::chrono::seconds::period>(newTime - startTime)
                  .count();
        int frameIndex = renderer_.getFrameIndex();
        FrameInfo frameInfo{frameIndex, commandBuffer, globalDescriptorSets[frameIndex]};

        uboBuffers[frameIndex]->writeToBuffer(&ubo);
        uboBuffers[frameIndex]->flush();

        renderer_.beginSwapChainRenderPass(commandBuffer);

        menuSystem.render(frameInfo);
        menuSystem2.render(frameInfo);

        simpleRenderSystem.renderGameObjects(frameInfo, gameObjects_);
        renderer_.endSwapChainRenderPass(commandBuffer);
        renderer_.endFrame(false);
      }
      computeShader.render(frameInfo);
      if ((std::chrono::duration<float, std::chrono::seconds::period>(newTime - startFrameTime)
               .count())
          >= 1.0) {
        window_.updateFrame(m_fpscount_);
        m_fpscount_ = 0;
        startFrameTime = std::chrono::high_resolution_clock::now();
      }
    }
    vkDeviceWaitIdle(device_.getDevice());
  }

  void Application::loadGameObjects() {
    life_ = {};

    float scale = 0.01;
    auto square = GameObject::createGameObject();
    square.color = colorLive_;
    life_ = Model::createSquareModel(scale);

    for (std::map<std::pair<int, int>, std::pair<int, std::vector<Model::Vertex>>>::iterator it
         = life_.begin();
         it != life_.end(); it++) {
      vertices_.push_back(it->second.second[0]);
      vertices_.push_back(it->second.second[1]);
      vertices_.push_back(it->second.second[2]);
      vertices_.push_back(it->second.second[3]);
      vertices_.push_back(it->second.second[4]);
      vertices_.push_back(it->second.second[5]);
    }
    std::cout << vertices_.size() << std::endl;
    square.model = std::make_unique<Model>(device_, vertices_);
    gameObjects_.push_back(std::move(square));
  }

}  // namespace ve