#pragma once
#include "device.hpp"
#include "swap_chain.hpp"

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

    SwapChain _swapChain{window.getExtent(), device};
  };
}  // namespace ve