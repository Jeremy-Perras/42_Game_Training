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
    SwapChain(VkExtent2D windowExtent, Device &device);
    ~SwapChain();
    void createImageViews();
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  private:
    void createSwapChain();
    VkExtent2D _windowExtent;
    Device &_device;
    VkSwapchainKHR _swapChain;
    std::vector<VkImage> _swapChainImages;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;
    std::vector<VkImageView> _swapChainImageViews;
  };

}  // namespace ve