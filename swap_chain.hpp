#pragma once

#include "device.hpp"
#include "vulkan/vulkan_core.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace ve {

  class SwapChain {
  public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    SwapChain(Device &deviceRef, VkExtent2D windowExtent);
    SwapChain(Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
    ~SwapChain();

    SwapChain(const SwapChain &) = delete;
    SwapChain operator=(const SwapChain &) = delete;

    float extentAspectRatio() const {
      return static_cast<float>(swapChainExtent_.width)
             / static_cast<float>(swapChainExtent_.height);
    }
    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex, bool compute);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t const *imageIndex);
    bool compareSwapFormat(const SwapChain &swapChain) {
      return swapChain.swapChainDepthFormat == swapChainDepthFormat
             && swapChain.swapChainImageFormat_ == swapChainImageFormat_;
    }
    VkResult submitComputeCommandBuffers(const VkCommandBuffer *buffers,
                                         uint32_t const *imageIndex);

    // compute
    void computeWait();
    void computeResetFences();
    size_t getComputeCurrentFrame() const { return currentComputeFrame_; }
    // size_t getCurrentFrame() const { return currentFrame_; }
    void computeQueueSubmit(const VkCommandBuffer *buffers);

    // Getter
    VkFormat getSwapChainImageFormat() { return swapChainImageFormat_; }
    VkExtent2D getSwapChainExtent() { return swapChainExtent_; }
    uint32_t getWidth() const { return swapChainExtent_.width; }
    uint32_t getHeight() const { return swapChainExtent_.height; }
    VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers_[index]; }
    VkRenderPass getRenderPass() { return renderPass_; }
    VkImageView getImageView(int index) { return swapChainImageViews_[index]; }
    size_t getImageCount() { return swapChainImages_.size(); }
    void createImageViews();

  private:
    void init();
    void createSwapChain();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat swapChainImageFormat_;
    VkFormat swapChainDepthFormat;
    VkExtent2D swapChainExtent_;

    std::vector<VkFramebuffer> swapChainFramebuffers_;
    VkRenderPass renderPass_;

    std::vector<VkImage> depthImages_;
    std::vector<VkDeviceMemory> depthImageMemorys_;
    std::vector<VkImageView> depthImageViews_;
    std::vector<VkImage> swapChainImages_;
    std::vector<VkImageView> swapChainImageViews_;

    Device &device_;
    VkExtent2D windowExtent_;

    VkSwapchainKHR swapChain_;
    std::shared_ptr<SwapChain> oldSwapChain_;

    std::vector<VkSemaphore> imageAvailableSemaphores_;
    std::vector<VkSemaphore> renderFinishedSemaphores_;

    std::vector<VkFence> inFlightFences_;
    std::vector<VkFence> imagesInFlight_;

    // TODO
    std::vector<VkFence> computeInFlightFences;
    std::vector<VkSemaphore> computeFinishedSemaphores;

    // size_t currentFrame_ = 0;
    size_t currentComputeFrame_ = 0;
  };

}  // namespace ve
