#pragma once

// lib
#include <vulkan/vulkan.h>

#include <chrono>
namespace ve {
  struct FrameInfo {
    int frameIndex;
    VkCommandBuffer commandBuffer;
    VkDescriptorSet globalDescriptorSet;
    double Time;
  };
}  // namespace ve