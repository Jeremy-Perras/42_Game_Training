#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <stdexcept>

#include "game_object.hpp"
#include "renderer.hpp"

namespace ve {
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

    static long double getElapsedTime(struct timeval end, struct timeval begin);

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    Renderer renderer_{window_, device_};
    std::vector<GameObject> gameObjects_;

    unsigned int m_fpscount_;
    struct timeval start_;
    struct timeval end_;
  };
}  // namespace ve