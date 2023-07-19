#include "app.hpp"

#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <ostream>
#include <utility>

#include "buffer.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"
#include "menu.hpp"
#include "model.hpp"
#include "simple_render_system.hpp"
namespace ve {
  struct GlobalUbo {
    float deltaTime;
  };

  Application::Application() : m_fpscount_(0) {
    gettimeofday(&start_, NULL);
    gettimeofday(&end_, NULL);

    // globalPool_
    //     = DescriptorPool::Builder(device_)
    //           .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
    //           .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
    //           .build();
    texturePool_
        = DescriptorPool::Builder(device_)
              .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
              .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
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

    // auto globalSetLayout
    //     = DescriptorSetLayout::Builder(device_)
    //           .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
    //           .build();

    auto textureSetLayout = DescriptorSetLayout::Builder(device_)
                                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                            VK_SHADER_STAGE_FRAGMENT_BIT)
                                .build();

    // std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    std::vector<VkDescriptorSet> textureDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < static_cast<int>(textureDescriptorSets.size()); i++) {
      auto bufferInfo = uboBuffers[i]->descriptorInfo();
      DescriptorWriter(*textureSetLayout, *texturePool_)
          .writeBuffer(0, &bufferInfo)
          .build(textureDescriptorSets[i]);
    }
    // for (int i = 0; i < static_cast<int>(textureDescriptorSets.size()); i++) {
    //   auto bufferInfo = uboBuffers[i]->descriptorInfo();
    //   DescriptorWriter(*textureSetLayoutSetLayout, *globalPool_)
    //       .writeBuffer(0, &bufferInfo)
    //       .build(globalDescriptorSets[i]);
    // }
    MenuSystem menuSystem{device_,
                          renderer_.getSwapChainRenderPass(),
                          textureSetLayout->getDescriptorSetLayout(),
                          {{{-0.5F, -0.5F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F}},
                           {{0.5F, -0.5F}, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F}},
                           {{0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F}},
                           {{-0.5F, 0.5F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F}}}};
    // SimpleRenderSystem simpleRenderSystem{device_, renderer_.getSwapChainRenderPass(),
    //                                       textureSetLayout->getDescriptorSetLayout()};
    // KeyboardMovementController playerController{};
    struct timeval gameStart;
    gettimeofday(&gameStart, NULL);
    auto startTime = std::chrono::high_resolution_clock::now();

    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();
      m_fpscount_++;

      // playerController.moveInPlaneXZ(window_.getGLFWwindow(), gameObjects_[0]);

      if (auto* commandBuffer = renderer_.beginFrame()) {
        int frameIndex = renderer_.getFrameIndex();
        FrameInfo frameInfo{frameIndex, commandBuffer, textureDescriptorSets[frameIndex]};
        auto newTime = std::chrono::high_resolution_clock::now();

        GlobalUbo ubo{};
        ubo.deltaTime
            = std::chrono::duration<float, std::chrono::seconds::period>(newTime - startTime)
                  .count();

        uboBuffers[frameIndex]->writeToBuffer(&ubo);
        uboBuffers[frameIndex]->flush();

        // renderer_.beginSwapChainRenderPass(commandBuffer);
        menuSystem.render(frameInfo);
        // simpleRenderSystem.renderGameObjects(frameInfo, gameObjects_);
        // renderer_.endSwapChainRenderPass(commandBuffer);
        // renderer_.endFrame();
      }

      if (getElapsedTime(end_, start_) >= 1000) {
        window_.updateFrame(m_fpscount_);
        m_fpscount_ = 0;
        gettimeofday(&start_, NULL);
      }

      gettimeofday(&end_, NULL);
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
    square.model = std::make_unique<Model>(device_, vertices_, renderer_);
    gameObjects_.push_back(std::move(square));
  }

  long double Application::getElapsedTime(struct timeval end, struct timeval begin) {
    long double elapsed;
    elapsed = static_cast<long double>(end.tv_sec - begin.tv_sec) * 1000.0
              + static_cast<long double>(end.tv_usec - begin.tv_usec) / 1000.0;
    return (elapsed);
  }

}  // namespace ve