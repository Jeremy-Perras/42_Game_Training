#pragma once
#include <memory>
#include <stdexcept>
#include <vector>

#include "device.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "vulkan/vulkan_core.h"
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
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    SwapChain swapChain_{device_, window_.getExtent()};
    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;
    std::vector<VkCommandBuffer> commandBuffers_;
  };
}  // namespace ve