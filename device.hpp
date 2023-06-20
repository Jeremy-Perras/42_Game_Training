#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>

#include "set"
#include "vulkan/vulkan_core.h"
#include "window.hpp"
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace ve {

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };
  class Device {
  public:
    Device(Window &window);
    ~Device();
    Device(Device const &rhs) = delete;
    Device &operator=(Device const &src) = delete;

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceKHR getSurface() { return _surface; }
    VkDevice getDevice() { return _device; }
    VkPhysicalDevice getPhysicalDevice() { return _physicalDevice; }
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  private:
    void createInstance();
    bool checkValidationLayerSupport();
    void setupDebugMessenger();
    void static populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void pickPhysicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);
    void createLogicalDevice();
    void createSurface();
    VkDevice _device;
    Window &_window;
    const std::vector<const char *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};
    VkInstance _instance;
    VkDebugUtilsMessengerEXT _callback;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkQueue _graphicsQueue;
    VkSurfaceKHR _surface;
    VkQueue _presentQueue;
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    const std::vector<const char *> _deviceExtensions
        = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};
  };

}  // namespace ve