#include "app.hpp"

#include <sys/_types/_int32_t.h>

#include <algorithm>
#include <chrono>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <ostream>
#include <utility>

#include "buffer.hpp"
#include "camera.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"
#include "model.hpp"
#include "simple_render_system.hpp"
#include "swap_chain.hpp"
#include "vulkan/vulkan_core.h"
namespace ve {

  struct GlobalUbo {
    glm::mat4 projectionView{1.0F};
    glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0F, -3.0F, -1.0F});
  };

  Application::Application() : m_fpscount_(0) {
    gettimeofday(&start_, NULL);
    gettimeofday(&end_, NULL);
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
    SimpleRenderSystem simpleRenderSystem(device_, renderer_.getSwapChainRenderPass());
    Camera camera{};
    camera.setViewTarget(glm::vec3(-1.0F, -2.0F, 2.0F), glm::vec3(0.0F, 0.0F, 2.5F));

    struct timeval gameStart;
    gettimeofday(&gameStart, NULL);
    auto currentTime = std::chrono::high_resolution_clock::now();

    auto viewerObject = GameObject::createGameObject();
    KeyboardMovementController cameraController{};

    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();

      auto newTime = std::chrono::high_resolution_clock::now();
      float frameTime
          = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime)
                .count();

      cameraController.moveInPlaneXZ(window_.getGLFWwindow(), frameTime, viewerObject);
      camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

      currentTime = newTime;
      m_fpscount_++;

      float aspect = renderer_.getAspectRatio();

      camera.setPerspectiveProjection(glm::radians(50.F), aspect, 0.1F, 10.F);

      if (auto* commandBuffer = renderer_.beginFrame()) {
        int frameIndex = renderer_.getFrameIndex();
        // update
        FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera};
        GlobalUbo ubo{};
        ubo.projectionView = camera.getProjection() * camera.getView();
        uboBuffers[frameIndex]->writeToBuffer(&ubo);
        uboBuffers[frameIndex]->flush();
        // render
        renderer_.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(frameInfo, gameObjects_);
        renderer_.endSwapChainRenderPass(commandBuffer);
        renderer_.endFrame();
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
    std::shared_ptr<Model> model = Model::createModelFromFile(
        device_, "/Users/jeremyperras/Desktop/GameEngine/models/flat_vase.obj");
    auto gameObj = GameObject::createGameObject();

    gameObj.model = model;
    gameObj.transform.translation = {0.5F, 0.5F, 2.5};
    gameObj.transform.scale = {3.F, 1.5F, 3.F};

    gameObjects_.push_back(std::move(gameObj));
  }

  long double Application::getElapsedTime(struct timeval end, struct timeval begin) {
    long double elapsed;
    elapsed = static_cast<long double>(end.tv_sec - begin.tv_sec) * 1000.0
              + static_cast<long double>(end.tv_usec - begin.tv_usec) / 1000.0;
    return (elapsed);
  }

}  // namespace ve