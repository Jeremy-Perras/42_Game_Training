#pragma once

#include <_types/_uint32_t.h>

#include <cassert>
#include <glm/gtc/constants.hpp>
#include <map>

#include "buffer.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "swap_chain.hpp"
#include "vulkan/vulkan_core.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace ve {
  class Model {
  public:
    struct Vertex {
      glm::vec2 position;
      glm::vec3 colors;
      // getters
      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    // Model(Device &device, const std::vector<Vertex> &vertices);

    Model(Device &device, const std::vector<Vertex> &vertices);

    Model(const Model &src) = delete;
    Model &operator=(const Model &rhs) = delete;
    ~Model();

    static std::unique_ptr<Model> createSquareModel(Device &device);
    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;

  private:
    const std::vector<Vertex> vertices_;
    void createVertexBuffer(const std::vector<Vertex> &vertices);
    VkImageView createImageView(VkImage image, VkFormat format);
    void createIndexBuffers(const std::vector<uint32_t> &indices);

    Device &device_;

    std::unique_ptr<Buffer> vertexBuffer_;

    bool hasIndexBuffer_ = false;
    std::unique_ptr<Buffer> indexBuffer_;
    uint32_t indexCount_;

    uint32_t vertexCount_;

    VkBuffer stagingBuffer_;
    VkDeviceMemory stagingBufferMemory_;
  };

}  // namespace ve