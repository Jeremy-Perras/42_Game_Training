#include "app.hpp"

#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <utility>

#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"
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
    SimpleRenderSystem simpleRenderSystem(device_, renderer_.getSwapChainRenderPass());
    // KeyboardMovementController playerController{};

    while (static_cast<int>(window_.shouldClose()) == 0
           && static_cast<int>(glfwGetKey(window_.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                  == 0) {
      glfwPollEvents();
      m_fpscount_++;

      // playerController.moveInPlaneXZ(window_.getGLFWwindow(), gameObjects_[0]);

      if (auto* commandBuffer = renderer_.beginFrame()) {
        renderer_.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects_);
        renderer_.endSwapChainRenderPass(commandBuffer);
        renderer_.endFrame();
      }
      if (getElapsedTime(end_, start_) >= 1000) {
        window_.updateFrame(m_fpscount_);
        m_fpscount_ = 0;
        gettimeofday(&start_, NULL);
        Application::gameLife();
      }

      gettimeofday(&end_, NULL);
    }
    vkDeviceWaitIdle(device_.getDevice());
  }

  void Application::loadGameObjects() {
    float x = 0;
    float y = 0;
    float scale = 0.01;
    while (-1 + scale * y < 1.0F) {
      while (-1 + scale * x < 1.0F) {
        life_.insert(std::pair<std::pair<float, float>, float>({x, y}, 0));
        x++;
        auto square = GameObject::createGameObject();
        square.transform.scale = glm::vec2(scale);
        square.transform.translation = {x * scale - 1, y * scale - 1};
        square.color = {1.F, 0.F, 0.F};
        square.model = Model::createSquareModel(device_, {.5f, .0f});
        gameObjects_.push_back(std::move(square));
      }
      xMax_ = x;
      x = 0;
      y++;
    }
    yMax_ = y;
    std::cout << xMax_ << " " << yMax_ << std::endl;
    life_.find({100 - 1, 100})->second = 1;
    life_.find({100, 100})->second = 1;
    life_.find({100 + 1, 100})->second = 1;
  }

  long double Application::getElapsedTime(struct timeval end, struct timeval begin) {
    long double elapsed;
    elapsed = static_cast<long double>(end.tv_sec - begin.tv_sec) * 1000.0
              + static_cast<long double>(end.tv_usec - begin.tv_usec) / 1000.0;
    return (elapsed);
  }

  // void Application::gameLife() {
  //   int countOne = 0;
  //   float scale = 0.01;

  //   for (auto p : life_) {
  //     if (p.second == 0) {
  //       countOne = life_.find({p.first.first - 1, p.first.second - 1})->second == 1 ? countOne
  //       + 1
  //                                                                                   : countOne;
  //       countOne = life_.find({p.first.first, p.first.second - 1})->second == 1 ? countOne + 1
  //                                                                               : countOne;

  //       countOne = life_.find({p.first.first + 1, p.first.second - 1})->second == 1 ? countOne
  //       + 1
  //                                                                                   : countOne;
  //       countOne = life_.find({p.first.first - 1, p.first.second})->second == 1 ? countOne + 1
  //                                                                               : countOne;

  //       countOne = life_.find({p.first.first + 1, p.first.second})->second == 1 ? countOne + 1
  //                                                                               : countOne;

  //       countOne = life_.find({p.first.first - 1, p.first.second + 1})->second == 1 ? countOne
  //       + 1
  //                                                                                   : countOne;

  //       countOne = life_.find({p.first.first, p.first.second + 1})->second == 1 ? countOne + 1
  //                                                                               : countOne;

  //       countOne = life_.find({p.first.first + 1, p.first.second + 1})->second == 1 ? countOne
  //       + 1
  //                                                                                   : countOne;
  //       if (countOne == 3) {
  //         p.second = 1;
  //         std::cout << "1 : " << p.first.first * scale - 1 << " " << p.first.second * scale - 1
  //                   << std::endl;
  //         auto square = GameObject::createGameObject();
  //         square.transform.scale = glm::vec2(scale);
  //         square.transform.translation = {
  //             p.first.first * scale - 1,
  //             p.first.second * scale - 1,
  //         };
  //         square.color = {1.F, 0.F, 0.F};
  //         square.model = Model::createSquareModel(device_, {.5f, .0f});
  //         gameObjects_.push_back(std::move(square));

  //       } else {
  //         p.second = 0;
  //       }
  //     } else if (p.second == 1) {
  //       countOne = life_.find({p.first.first - 1, p.first.second - 1})->second == 1 ? countOne
  //       + 1
  //                                                                                   : countOne;
  //       countOne = life_.find({p.first.first, p.first.second - 1})->second == 1 ? countOne + 1
  //                                                                               : countOne;

  //       countOne = life_.find({p.first.first + 1, p.first.second - 1})->second == 1 ? countOne
  //       + 1
  //                                                                                   : countOne;
  //       countOne = life_.find({p.first.first - 1, p.first.second})->second == 1 ? countOne + 1
  //                                                                               : countOne;

  //       countOne = life_.find({p.first.first + 1, p.first.second})->second == 1 ? countOne + 1
  //                                                                               : countOne;

  //       countOne = life_.find({p.first.first - 1, p.first.second + 1})->second == 1 ? countOne
  //       + 1
  //                                                                                   : countOne;

  //       countOne = life_.find({p.first.first, p.first.second + 1})->second == 1 ? countOne + 1
  //                                                                               : countOne;

  //       countOne = life_.find({p.first.first + 1, p.first.second + 1})->second == 1 ? countOne
  //       + 1
  //                                                                                   : countOne;
  //       if (countOne == 3 || countOne == 2) {
  //         p.second = 1;
  //         std::cout << p.first.first << " " << p.first.second << std::endl;
  //         auto square = GameObject::createGameObject();
  //         square.transform.scale = glm::vec2(scale);
  //         square.transform.translation = {
  //             p.first.first * scale - 1,
  //             p.first.second * scale - 1,
  //         };
  //         square.color = {1.F, 0.F, 0.F};
  //         square.model = Model::createSquareModel(device_, {.5f, .0f});
  //         gameObjects_.push_back(std::move(square));

  //       } else {
  //         p.second = 0;
  //       }
  //     }

  //     countOne = 0;
  //   }
  // }
  void Application::gameLife() {
    for (auto& p : gameObjects_) {
      // auto test = std::find(gameObjects_.begin(), gameObjects_.end(), ) != vec.end();
      std::cout << p.getId() << std::endl;
    };
  }
}  // namespace ve