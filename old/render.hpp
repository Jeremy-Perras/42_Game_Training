#pragma once

#include <vector>

#include "device.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "vulkan/vulkan_core.h"
namespace ve {
  class Render {
  public:
    Render(Window &_window, Device &_device);
    ~Render();
    Render(Render const &rhs) = delete;
    Render &operator=(Render const &src) = delete;

  private:
    Window &_window;
    Device &_device;

    std::unique_ptr<SwapChain> _swapChain;
    std::unique_ptr<Pipeline> _pipeline;

    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    std::vector<VkCommandBuffer> _commandBuffers;
  };

}  // namespace ve