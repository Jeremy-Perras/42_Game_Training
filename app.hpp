#pragma once
#include "device.hpp"

namespace ve {
  class Application {
  public:
    void run();
    Application(){};

  private:
    void initVulkan();
    void mainLoop();
    Window window;
    Device device{window};
  };
}  // namespace ve