#pragma once
#include <_types/_uint32_t.h>

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"
class Device;
struct SwapChainSupportDetails;

namespace ve {
  class SwapChain {
  public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    SwapChain(VkExtent2D windowExtent, Device &device);
    ~SwapChain();
    SwapChain(SwapChain const &rhs) = delete;
    SwapChain &operator=(SwapChain const &src) = delete;

    void createImageViews();
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkExtent2D getSwapChainExtent() { return _swapChainExtent; }
    VkRenderPass getRenderPass() { return _renderPass; }
    VkFramebuffer getFrameBuffer(uint32_t currentImageIndex) {
      return _swapChainFramebuffers[currentImageIndex];
    }

  private:
    void createSwapChain();
    void createRenderPass();
    void createFramebuffers();

    VkExtent2D _windowExtent;
    Device &_device;
    VkSwapchainKHR _swapChain;
    std::vector<VkImage> _swapChainImages;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;
    std::vector<VkImageView> _swapChainImageViews;
    VkRenderPass _renderPass;
    std::vector<VkFramebuffer> _swapChainFramebuffers;
  };

}  // namespace ve