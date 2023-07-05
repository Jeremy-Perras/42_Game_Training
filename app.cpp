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

      Application::gameLife();
      gameObjects_[0].model->updateVertexBuffer(vertices_);
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
    square.model = std::make_unique<Model>(device_, vertices_);
    gameObjects_.push_back(std::move(square));
  }

  long double Application::getElapsedTime(struct timeval end, struct timeval begin) {
    long double elapsed;
    elapsed = static_cast<long double>(end.tv_sec - begin.tv_sec) * 1000.0
              + static_cast<long double>(end.tv_usec - begin.tv_usec) / 1000.0;
    return (elapsed);
  }

  void Application::gameLife() {
    int countOne = 0;
    int i = 0;
    std::map<std::pair<int, int>, std::pair<int, std::vector<Model::Vertex>>> lifeOld = life_;

    for (const auto& p : life_) {
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
          for (int j = 0; j < 6; j++) {
            vertices_[i + j].colors = colorLive_;
          }

          life_.find({p.first.first, p.first.second})->second.first = 1;
        } else {
          for (int j = 0; j < 6; j++) {
            vertices_[i + j].colors = colorDead_;
          }
          life_.find({p.first.first, p.first.second})->second.first = 0;
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
          for (int j = 0; j < 6; j++) {
            vertices_[i + j].colors = colorLive_;
          }
        } else {
          life_.find({p.first.first, p.first.second})->second.first = 0;
          for (int j = 0; j < 6; j++) {
            vertices_[i + j].colors = colorDead_;
          }
        }
      }
      i += 6;
      countOne = 0;
    }
  }

}  // namespace ve