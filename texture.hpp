#pragma once
#include <memory>

#include "device.hpp"

namespace ve {
  class Texture {
  public:
    Texture(Device &device, const std::string &textureFilepath);
    Texture(const Texture &src) = delete;
    Texture &operator=(const Texture &rhs) = delete;
    ~Texture();
    // Getter
    VkDescriptorImageInfo getImageInfo() {
      return VkDescriptorImageInfo{
          textureSampler_,
          textureImageView_,
          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      };
    }

  private:
    void createTextureImage(const std::string &textureFilepath);
    void createTextureImageView();
    void createTextureSampler();
    VkImageView createImageView(VkImage image, VkFormat format);
    Device &device_;
    VkImage textureImage_;
    VkDeviceMemory textureImageMemory_;
    VkImageView textureImageView_;
    VkSampler textureSampler_;
  };
}  // namespace ve