#pragma once

// lib
#include <vulkan/vulkan.h>

namespace ve {
  struct FrameInfo {
    size_t frameIndex;
    VkCommandBuffer commandBuffer;
    VkDescriptorSet descriptorSet;
    double Time;
  };
}  // namespace ve