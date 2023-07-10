#pragma once

#include <vulkan/vulkan.h>

#include "camera.hpp"
#include "game_object.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    VkDescriptorSet globalDescriptorSet;
    GameObject::Map& gameObject;
  };
}  // namespace ve