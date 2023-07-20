#include "model.hpp"

#include <cstddef>
#include <cstring>
#include <memory>

#include "device.hpp"
#include "renderer.hpp"
#include "swap_chain.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {

  Model::Model(Device &device, const std::vector<Vertex> &vertices) : device_(device) {
    createVertexBuffer(vertices);
  }

  Model::~Model() {}

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

    if (!hasIndexBuffer_) {
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

}  // namespace ve