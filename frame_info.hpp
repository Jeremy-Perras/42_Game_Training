#pragma once

// lib
#include <vulkan/vulkan.h>

namespace ve {
  struct FrameInfo {
    int frameIndex;
    VkCommandBuffer commandBuffer;
    VkDescriptorSet globalDescriptorSet;
  };
}  // namespace ve