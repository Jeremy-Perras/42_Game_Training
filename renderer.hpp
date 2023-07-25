#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>

#include "swap_chain.hpp"

namespace ve {
  class Renderer {
  public:
    Renderer(Window &window, Device &device);
    Renderer(const Renderer &src) = delete;
    Renderer &operator=(const Renderer &rhs) = delete;
    ~Renderer();

    VkCommandBuffer beginFrame(bool compute);
    void endFrame(bool compute);
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer) const;

    // compute
    void computeWait() { swapChain_->computeWait(); };
    void computeResetFences() { swapChain_->computeResetFences(); }
    size_t getComputeCurrentFrame() const { return swapChain_->getComputeCurrentFrame(); }
    void computeQueueSubmit(const VkCommandBuffer *buffers) {
      swapChain_->computeQueueSubmit(buffers);
    }

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

    SwapChain &getSwapChain() { return *swapChain_; }

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    Window &window_;
    Device &device_;
    std::unique_ptr<SwapChain> swapChain_;
    std::vector<VkCommandBuffer> commandBuffers_;
    std::vector<VkCommandBuffer> computeCommandBuffers_;
    uint32_t currentImageIndex_{0};
    int currentFrameIndex_{0};
    bool isFrameStarted_{false};
  };
}  // namespace ve