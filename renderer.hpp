#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <cassert>
#include <stdexcept>

#include "swap_chain.hpp"

namespace ve {
  class Renderer {
  public:
    Renderer(Window &window, Device &device);
    Renderer(const Renderer &src) = delete;
    Renderer &operator=(const Renderer &rhs) = delete;
    ~Renderer();

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    // getters
    bool isFrameInProgress() const { return isFrameStarted_; }
    VkRenderPass getSwapChainRenderPass() const { return swapChain_->getRenderPass(); }
    VkCommandBuffer getCurrentCommandBuffer() const {
      assert(isFrameStarted_ && " Cannot get command buffer when freame not in progress");
      return commandBuffers_[currentFrameIndex_];
    }
    int getFrameIndex() const {
      assert(isFrameStarted_ && "Cannot get Frame Index when frame not in progress");

      return currentFrameIndex_;
    }
    float getAspectRatio() const { return swapChain_->extentAspectRatio(); }

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    Window &window_;
    Device &device_;
    std::unique_ptr<SwapChain> swapChain_;
    std::vector<VkCommandBuffer> commandBuffers_;
    uint32_t currentImageIndex_{0};
    int currentFrameIndex_{0};
    bool isFrameStarted_{false};
  };
}  // namespace ve