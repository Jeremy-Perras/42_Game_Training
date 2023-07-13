#include "texture.hpp"

#include <cstddef>
#include <stdexcept>

#include "device.hpp"

namespace ve {
  void Texture::createTextureImage() {
    int texWidth;
    int texHeight;
    int texChannels;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    stbi_uc* pixels
        = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth * texHeight * 4);

    if (pixels == nullptr) {
      throw std::runtime_error("failed to load texture image!");
    }

    device_.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(device_.getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device_.getDevice(), stagingBufferMemory);
    stbi_image_free(pixels);
  }
}  // namespace ve