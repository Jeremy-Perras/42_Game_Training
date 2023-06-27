#include "app.hpp"

#include <glm/gtc/constants.hpp>
#include <memory>

#include "game_object.hpp"
#include "model.hpp"
#include "rainbow.hpp"
#include "simple_render_system.hpp"
namespace ve {

  Application::Application() : m_fpscount_(0) {
    gettimeofday(&start_, NULL);
    gettimeofday(&end_, NULL);
    loadGameObjects();
  }

  Application::~Application() {}

  void Application::mainLoop() {
    RainbowSystem rainbow(10);
    SimpleRenderSystem simpleRenderSystem(device_, renderer_.getSwapChainRenderPass());
    while (static_cast<int>(window_.shouldClose()) == 0) {
      m_fpscount_++;
      if (getElapsedTime(end_, start_) >= 1000) {
        window_.updateFrame(m_fpscount_);
        m_fpscount_ = 0;
        gettimeofday(&start_, NULL);
      }
      glfwPollEvents();
      if (auto* commandBuffer = renderer_.beginFrame()) {
        renderer_.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects_);
        renderer_.endSwapChainRenderPass(commandBuffer);
        renderer_.endFrame();
      }
      gettimeofday(&end_, NULL);
    }
    vkDeviceWaitIdle(device_.getDevice());
  }

  void Application::loadGameObjects() {
    std::vector<Model::Vertex> vertices = {
        {{-1.F, -1.F}, {1.0F, 0.0F, 0.0F}}, {{1.F, 1.F}, {1.0F, 0.0F, 0.0F}},
        {{1.F, -1.F}, {1.0F, 0.0F, 0.0F}},  {{-1.F, -1.F}, {1.0F, 0.0F, 0.0F}},
        {{1.F, 1.F}, {1.0F, 0.0F, 0.0F}},   {{-1.F, 1.F}, {1.0F, 0.0F, 0.0F}},  //
    };
    auto model_ = std::make_shared<Model>(device_, vertices);
    auto triangle = GameObject::createGameObject();

    triangle.model = model_;
    triangle.color = {0.1F, 0.8F, 0.1F};
    triangle.transform2d.translation.x = 0.0F;
    triangle.transform2d.scale = {1.0F, 1.0F};
    triangle.transform2d.rotation = 0.0F * glm ::two_pi<float>();

    // gameObjects_.push_back(std::move(triangle));

    auto circle = GameObject::createGameObject();
    circle.transform2d.scale = glm::vec2{.05f};
    circle.transform2d.translation = {.5f, .5f};
    circle.color = {1.F, 0.F, 0.F};
    circle.model = Model::createCircleModel(device_, 64);
    gameObjects_.push_back(std::move(circle));
  }

  long double Application::getElapsedTime(struct timeval end, struct timeval begin) {
    long double elapsed;
    elapsed = static_cast<long double>(end.tv_sec - begin.tv_sec) * 1000.0
              + static_cast<long double>(end.tv_usec - begin.tv_usec) / 1000.0;
    return (elapsed);
  }

}  // namespace ve