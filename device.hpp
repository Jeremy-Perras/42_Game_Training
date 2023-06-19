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
  class Device {
  public:
    Device(Window &window);
    ~Device();
    Device(Device const &rhs) = delete;
    Device &operator=(Device const &src) = delete;
    void createInstance();
    bool checkValidationLayerSupport();
    void setupDebugMessenger();
    void static populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void pickPhysicalDevice();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    void createLogicalDevice();
    void createSurface();

  private:
    VkDevice device;
    Window &window;
    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    VkInstance instance;
    VkDebugUtilsMessengerEXT callback;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkQueue graphicsQueue;
    VkSurfaceKHR surface;
    VkQueue presentQueue;
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  };

}  // namespace ve