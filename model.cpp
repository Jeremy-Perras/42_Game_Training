#include "model.hpp"

#include <cstddef>
#include <cstring>
#include <memory>

#include "device.hpp"
#include "renderer.hpp"
#include "swap_chain.hpp"
#include "vulkan/vulkan_core.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace ve {
  // Model::Model(Device &device, const std::vector<Vertex> &vertices)
  //     : device_(device), renderer_() {
  //   createTextureImage();
  //   createTextureImageView();
  //   createVertexBuffer(vertices);
  // }

  Model::Model(Device &device, const std::vector<Vertex> &vertices, Renderer &renderer)
      : device_(device), renderer_(renderer) {
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    createVertexBuffer(vertices);
  }

  Model::~Model() {
    vkDestroySampler(device_.getDevice(), textureSampler_, nullptr);
    vkDestroyImageView(device_.getDevice(), textureImageView_, nullptr);
    vkDestroyImage(device_.getDevice(), textureImage_, nullptr);
    vkFreeMemory(device_.getDevice(), textureImageMemory_, nullptr);
    // vkDestroyBuffer(device_.getDevice(), vertexBuffer_, nullptr);
    // vkFreeMemory(device_.getDevice(), vertexBufferMemory_, nullptr);
  }

  void Model::createVertexBuffer(const std::vector<Vertex> &vertices) {
    vertexCount_ = static_cast<uint32_t>(vertices.size());
    assert(vertexCount_ >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;
    uint32_t vertexSize = sizeof(vertices[0]);

    Buffer stagingBuffer{
        device_,
        vertexSize,
        vertexCount_,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)vertices.data());

    vertexBuffer_ = std::make_unique<Buffer>(
        device_, vertexSize, vertexCount_,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer_->getBuffer(), bufferSize);
  }

  void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
    indexCount_ = static_cast<uint32_t>(indices.size());
    hasIndexBuffer_ = indexCount_ > 0;

    if (!indexBuffer_) {
      return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount_;
    uint32_t indexSize = sizeof(indices[0]);

    Buffer stagingBuffer{
        device_,
        indexSize,
        indexCount_,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)indices.data());

    indexBuffer_ = std::make_unique<Buffer>(
        device_, indexSize, indexCount_,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), indexBuffer_->getBuffer(), bufferSize);
  }

  void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer_->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
  }

  void Model::draw(VkCommandBuffer commandBuffer) const {
    vkCmdDraw(commandBuffer, vertexCount_, 1, 0, 0);
  }

  std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
  }

  std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, colors);

    return attributeDescriptions;
  }

  std::map<std::pair<int, int>, std::pair<int, std::vector<Model::Vertex>>>
  Model::createSquareModel(float scale) {
    float x = 0;
    float y = 0;
    int i = 0;
    std::vector<Model::Vertex> vertices = {};
    std::map<std::pair<int, int>, std::pair<int, std::vector<Model::Vertex>>> life;
    while (scale * y < 2.0F) {
      while (scale * x < 2.0F) {
        if ((x == 0) || (x == 1) || (y == 0) || (y == 1) || x == 199 || x == 200 || y == 199
            || y == 200) {
          vertices.push_back({{x * scale - 1.0F, y * scale - 1.0F}, {1, 1, 1}});
          vertices.push_back({{(x + 1.0F) * scale - 1.0F, y * scale - 1.0F}, {1, 1, 1}});
          vertices.push_back({{x * scale - 1.0F, (y + 1.0F) * scale - 1.0F}, {1, 1, 1}});

          vertices.push_back({{(x + 1.0F) * scale - 1.0F, y * scale - 1.0F}, {1, 1, 1}});
          vertices.push_back({{x * scale - 1.0F, (y + 1.0F) * scale - 1.0F}, {1, 1, 1}});
          vertices.push_back({{(x + 1.0F) * scale - 1.0F, (y + 1.0F) * scale - 1.0F}, {1, 1, 1}});
          life.insert(std::pair<std::pair<int, int>, std::pair<int, std::vector<Model::Vertex>>>(
              {x, y}, {1,
                       {vertices[i], vertices[i + 1], vertices[i + 2], vertices[i + 3],
                        vertices[i + 4], vertices[i + 5]}}));
        } else {
          vertices.push_back({{x * scale - 1.0F, y * scale - 1.0F}, {0.05, 0.05, 0.05}});
          vertices.push_back({{(x + 1.0F) * scale - 1.0F, y * scale - 1.0F}, {0.05, 0.05, 0.05}});
          vertices.push_back({{x * scale - 1.0F, (y + 1.0F) * scale - 1.0F}, {0.05, 0.05, 0.05}});

          vertices.push_back({{(x + 1.0F) * scale - 1.0F, y * scale - 1.0F}, {0.05, 0.05, 0.05}});
          vertices.push_back({{x * scale - 1.0F, (y + 1.0F) * scale - 1.0F}, {0.05, 0.05, 0.05}});
          vertices.push_back(
              {{(x + 1.0F) * scale - 1.0F, (y + 1.0F) * scale - 1.0F}, {0.05, 0.05, 0.05}});
          life.insert(std::pair<std::pair<int, int>, std::pair<int, std::vector<Model::Vertex>>>(
              {x, y}, {0,
                       {vertices[i], vertices[i + 1], vertices[i + 2], vertices[i + 3],
                        vertices[i + 4], vertices[i + 5]}}));
        }
        i = i + 6;
        x++;
      }
      x = 0;
      y++;
    }
    return life;
  }

  // void Model::updateVertexBuffer(const std::vector<Vertex> &vertices) {
  //   vertexCount_ = static_cast<uint32_t>(vertices.size());
  //   assert(vertexCount_ >= 3 && "Vertex count must be at least 3");
  //   VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;
  //   device_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  //                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
  //                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer_,
  //                        stagingBufferMemory_);

  //   void *data;
  //   vkMapMemory(device_.getDevice(), stagingBufferMemory_, 0, bufferSize, 0, &data);
  //   memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  //   vkUnmapMemory(device_.getDevice(), stagingBufferMemory_);
  //   auto *test = vertexBuffer_;
  //   auto *test3 = vertexBufferMemory_;
  //   device_.createBuffer(bufferSize,
  //                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
  //                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer_,
  //                        vertexBufferMemory_);

  //   device_.copyBuffer(stagingBuffer_, vertexBuffer_, bufferSize);
  //   vkDestroyBuffer(device_.getDevice(), stagingBuffer_, nullptr);
  //   vkFreeMemory(device_.getDevice(), stagingBufferMemory_, nullptr);
  //   vkDestroyBuffer(device_.getDevice(), test, nullptr);
  //   vkFreeMemory(device_.getDevice(), test3, nullptr);
  // }

  void Model::createTextureImage() {
    int texWidth;
    int texHeight;
    int texChannels;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    stbi_uc *pixels = stbi_load("texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize
        = static_cast<VkDeviceSize>(static_cast<long long>(texWidth) * texHeight * 4);

    if (pixels == nullptr) {
      throw std::runtime_error("failed to load texture image!");
    }

    device_.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory);
    void *data;
    vkMapMemory(device_.getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device_.getDevice(), stagingBufferMemory);
    stbi_image_free(pixels);

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(texWidth);
    imageInfo.extent.height = static_cast<uint32_t>(texHeight);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;  // Optionnel
    device_.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage_,
                                textureImageMemory_);
    device_.transitionImageLayout(textureImage_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    device_.copyBufferToImage(stagingBuffer, textureImage_, static_cast<uint32_t>(texWidth),
                              static_cast<uint32_t>(texHeight), 1);
    device_.transitionImageLayout(textureImage_, VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    vkDestroyBuffer(device_.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device_.getDevice(), stagingBufferMemory, nullptr);
  }

  void Model::createTextureImageView() {
    textureImageView_ = createImageView(textureImage_, VK_FORMAT_R8G8B8A8_SRGB);
  }

  VkImageView Model::createImageView(VkImage image, VkFormat format) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device_.getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
      throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
  }

  void Model::createTextureSampler() {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(device_.getPysicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(device_.getDevice(), &samplerInfo, nullptr, &textureSampler_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create texture sampler!");
    }
  }
}  // namespace ve