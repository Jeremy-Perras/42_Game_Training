#include "app.hpp"

#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <ostream>
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
    struct timeval gameStart;
    gettimeofday(&gameStart, NULL);
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
      }
      if (getElapsedTime(end_, gameStart) >= 3000) {
        Application::gameLife();
      }
      gettimeofday(&end_, NULL);
    }
    vkDeviceWaitIdle(device_.getDevice());
  }

  // void Application::loadGameObjects() {
  //   int x = 0;
  //   int y = 0;

  //   int xMax = 0;
  //   int yMax = 0;

  //   float scale = 0.01;
  //   while (-1 + scale * static_cast<float>(y) < 1.0F) {
  //     while (-1 + scale * static_cast<float>(x) < 1.0F) {
  //       auto square = GameObject::createGameObject();
  //       square.color = colorDead_;
  //       square.model = Model::createSquareModel(
  //           device_, {static_cast<float>(x) * scale - 1, static_cast<float>(y) * scale - 1});

  //       life_.insert(
  //           std::pair<std::pair<int, int>, std::pair<int, int>>({x, y}, {0, square.getId()}));
  //       gameObjects_.push_back(std::move(square));
  //       x++;
  //       xMax = x;
  //     }
  //     x = 0;
  //     y++;
  //   }
  //   yMax = y;
  //   for (int j = 0; j < yMax; j++) {
  //     for (int i = 0; i < xMax; i++) {
  //       if (i == 0 || i == 1 || j == 0 || j == 1 || j == yMax - 1 || j == yMax - 2 || i == xMax -
  //       1
  //           || i == xMax - 2) {
  //         life_.find({i, j})->second.first = 1;
  //         gameObjects_[life_.find({i, j})->second.second].color = colorLive_;
  //       }
  //     }
  //   }
  //   // for (auto obj : life_) {
  //   //   if (obj.first.first == 0 || obj.first.first == 1 || obj.first.first == xMax - 1
  //   //       || obj.first.first == xMax - 2 || obj.first.second == 0 || obj.first.second == 1
  //   //       || obj.first.first == yMax - 1 || obj.first.second == yMax - 2) {
  //   //     obj.second.first = 1;
  //   //     gameObjects_[obj.second.second].color = colorLive_;
  //   //   }
  //   // }
  // }
  void Application::loadGameObjects() {
    int x = 0;
    int y = 0;

    float scale = 0.01;
    while (-1 + scale * static_cast<float>(y) < 1.0F) {
      while (-1 + scale * static_cast<float>(x) < 1.0F) {
        auto square = GameObject::createGameObject();
        square.transform.scale = glm::vec2(scale);
        square.transform.translation
            = {static_cast<float>(x) * scale - 1, static_cast<float>(y) * scale - 1};
        square.color = colorDead_;
        square.model = Model::createSquareModel(device_, {.5f, .0f});
        life_.insert(
            std::pair<std::pair<int, int>, std::pair<int, int>>({x, y}, {0, square.getId()}));
        gameObjects_.push_back(std::move(square));
        x++;
      }
      x = 0;
      y++;
    }

    life_.find({100 - 1, 100})->second.first = 1;
    life_.find({100, 100})->second.first = 1;
    life_.find({100 + 1, 100})->second.first = 1;
    gameObjects_[life_.find({100 - 1, 100})->second.second].color = colorLive_;
    gameObjects_[life_.find({100, 100})->second.second].color = colorLive_;
    gameObjects_[life_.find({100 + 1, 100})->second.second].color = colorLive_;
  }

  long double Application::getElapsedTime(struct timeval end, struct timeval begin) {
    long double elapsed;
    elapsed = static_cast<long double>(end.tv_sec - begin.tv_sec) * 1000.0
              + static_cast<long double>(end.tv_usec - begin.tv_usec) / 1000.0;
    return (elapsed);
  }

  void Application::gameLife() {
    int countOne = 0;
    std::map<std::pair<int, int>, std::pair<int, int>> lifeOld = life_;

    for (auto p : life_) {
      if (p.second.first == 0) {
        countOne = lifeOld.find({p.first.first - 1, p.first.second - 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first - 1, p.first.second - 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first, p.first.second - 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first, p.first.second - 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first + 1, p.first.second - 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first + 1, p.first.second - 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first - 1, p.first.second}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first - 1, p.first.second})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first + 1, p.first.second}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first + 1, p.first.second})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first - 1, p.first.second + 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first - 1, p.first.second + 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first, p.first.second + 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first, p.first.second + 1})->second.first == 1
                       ? countOne + 1
                       : countOne;
        countOne = lifeOld.find({p.first.first + 1, p.first.second + 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first + 1, p.first.second + 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        if (countOne == 3) {
          gameObjects_[p.second.second].color = colorLive_;
          life_.find({p.first.first, p.first.second})->second.first = 1;
        } else {
          life_.find({p.first.first, p.first.second})->second.first = 0;
          gameObjects_[p.second.second].color = colorDead_;
        }
      } else if (p.second.first == 1) {
        countOne = lifeOld.find({p.first.first - 1, p.first.second - 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first - 1, p.first.second - 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first, p.first.second - 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first, p.first.second - 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first + 1, p.first.second - 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first + 1, p.first.second - 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first - 1, p.first.second}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first - 1, p.first.second})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first + 1, p.first.second}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first + 1, p.first.second})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first - 1, p.first.second + 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first - 1, p.first.second + 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        countOne = lifeOld.find({p.first.first, p.first.second + 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first, p.first.second + 1})->second.first == 1
                       ? countOne + 1
                       : countOne;
        countOne = lifeOld.find({p.first.first + 1, p.first.second + 1}) == lifeOld.end() ? countOne
                   : lifeOld.find({p.first.first + 1, p.first.second + 1})->second.first == 1
                       ? countOne + 1
                       : countOne;

        if (countOne == 3 || countOne == 2) {
          life_.find({p.first.first, p.first.second})->second.first = 1;
          gameObjects_[p.second.second].color = colorLive_;

        } else {
          life_.find({p.first.first, p.first.second})->second.first = 0;
          gameObjects_[p.second.second].color = colorDead_;
        }
      }

      countOne = 0;
    }
  }

}  // namespace ve