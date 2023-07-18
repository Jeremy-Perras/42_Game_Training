#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "descriptors.hpp"
#include "game_object.hpp"
#include "model.hpp"
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

    static constexpr int WIDTH = 2560;
    static constexpr int HEIGHT = 1600;
    void mainLoop();

    std::map<std::pair<int, int>, std::pair<int, std::vector<Model::Vertex>>> life_;
    void setLife(glm::vec3 color, int x, int y, int value);

  private:
    void loadGameObjects();
    void gameLife();
    void updateGame();
    static long double getElapsedTime(struct timeval end, struct timeval begin);

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    Renderer renderer_{window_, device_};
    std::vector<GameObject> gameObjects_;
    std::unique_ptr<DescriptorPool> globalPool_{};

    unsigned int m_fpscount_;
    struct timeval start_;
    struct timeval end_;
    glm::vec3 colorLive_ = {1, 1, 1};
    glm::vec3 colorDead_ = {0.05, 0.05, 0.05};
    std::vector<Model::Vertex> vertices_;
    int i_ = {0};
  };
}  // namespace ve