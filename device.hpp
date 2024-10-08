#pragma once

#include "window.hpp"

// std lib headers
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include <cassert>
namespace ve {

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
  };

  class Device {
  public:
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    Device(Window &window);
    ~Device();

    // Not copyable or movable
    Device(const Device &) = delete;
    Device operator=(const Device &) = delete;

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice_); }
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    // Buffer Helper Functions
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
                           uint32_t layerCount);

    void createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties,
                             VkImage &image, VkDeviceMemory &imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
                               VkImageLayout newLayout);
    VkPhysicalDeviceProperties properties;

    // Getter
    VkCommandPool getCommandPool() { return commandPool_; }
    SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice_); }
    VkDevice getDevice() { return device_; }
    VkSurfaceKHR getSurface() { return surface_; }
    VkQueue getGraphicsQueue() { return graphicsQueue_; }
    VkQueue getPresentQueue() { return presentQueue_; }
    VkQueue getComputeQueue() { return computeQueue_; }
    VkPhysicalDevice getPysicalDevice() { return physicalDevice_; }
    PFN_vkCmdPushDescriptorSetKHR getPushCommand() { return vkCmdPushDescriptorSetKHR; }

  private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

    // helper functions
    bool isDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char *> getRequiredExtensions() const;
    bool checkValidationLayerSupport();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void hasGflwRequiredInstanceExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    VkInstance instance_;
    VkDebugUtilsMessengerEXT debugMessenger_;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    Window &window_;
    VkCommandPool commandPool_;

    VkDevice device_;
    VkSurfaceKHR surface_;
    VkQueue graphicsQueue_;
    VkQueue presentQueue_;
    VkQueue computeQueue_;
    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions
        = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset",
           VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME};

    VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptorProps_{};
    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;
  };
}  // namespace ve