#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"

namespace ve {
  class Texture {
  public:
    Texture();
    Texture(const Texture &src) = delete;
    Texture &operator=(const Texture &rhs) = delete;
    ~Texture();
    void createTextureImage();
    // Getter
  private:
    Device &device_;
  };
}  // namespace ve