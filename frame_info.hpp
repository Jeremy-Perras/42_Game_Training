#pragma once

#include <vulkan/vulkan.h>

#include "camera.hpp"

namespace ve {
  struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
  };
}  // namespace ve