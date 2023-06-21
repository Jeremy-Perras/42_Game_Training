#pragma once
#include "device.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
namespace ve {
  class Application {
  public:
    void run();
    Application(){};

  private:
    void initVulkan();
    void drawFrame();
    void mainLoop();
    Window window;

    Device device{window};

    SwapChain _swapChain{window.getExtent(), device};
    Pipeline _pipeline{device, _swapChain.getSwapChainExtent(), _swapChain.getRenderPass()};
  };
}  // namespace ve