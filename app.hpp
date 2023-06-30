#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <map>
#include <stdexcept>
#include <utility>

#include "game_object.hpp"
#include "renderer.hpp"

namespace ve {
  // struct Point {
  //   double x, y;
  // };

  class Application {
  public:
    Application();
    Application(const Application &src) = delete;
    Application &operator=(const Application &rhs) = delete;
    ~Application();

    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    void mainLoop();

  private:
    void loadGameObjects();
    void gameLife();
    static long double getElapsedTime(struct timeval end, struct timeval begin);

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    Renderer renderer_{window_, device_};
    std::vector<GameObject> gameObjects_;

    std::map<std::pair<int, int>, std::pair<int, int>> life_;
    unsigned int m_fpscount_;
    struct timeval start_;
    struct timeval end_;
    glm::vec3 colorLive_ = {1, 1, 1};
    glm::vec3 colorDead_ = {0.05, 0.05, 0.05};
  };
}  // namespace ve