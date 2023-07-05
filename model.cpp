#include "model.hpp"

#include <cstddef>
#include <cstring>
#include <memory>

#include "device.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  Model::Model(Device &device, const std::vector<Vertex> &vertices) : device_(device) {
    createVertexBuffer(vertices);
  }
  Model::~Model() {
    vkDestroyBuffer(device_.getDevice(), vertexBuffer_, nullptr);
    vkFreeMemory(device_.getDevice(), vertexBufferMemory_, nullptr);
  }

  void Model::createVertexBuffer(const std::vector<Vertex> &vertices) {
    vertexCount_ = static_cast<uint32_t>(vertices.size());
    assert(vertexCount_ >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;

    device_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer_, stagingBufferMemory_);

    void *data;
    vkMapMemory(device_.getDevice(), stagingBufferMemory_, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(device_.getDevice(), stagingBufferMemory_);

    device_.createBuffer(bufferSize,
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer_, vertexBufferMemory_);

    device_.copyBuffer(stagingBuffer_, vertexBuffer_, bufferSize);
    vkDestroyBuffer(device_.getDevice(), stagingBuffer_, nullptr);
    vkFreeMemory(device_.getDevice(), stagingBufferMemory_, nullptr);
  }

  void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer_};
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

  void Model::sierpinski(std::vector<Model::Vertex> &vertices, int depth, glm::vec2 left,
                         glm::vec2 right, glm::vec2 top) {
    if (depth <= 0) {
      vertices.push_back({{top}, {1, 0, 0}});
      vertices.push_back({{right}, {1, 0, 0}});
      vertices.push_back({{left}, {1, 0, 0}});
    } else {
      auto leftTop = 0.5F * (left + top);
      auto rightTop = 0.5F * (right + top);
      auto leftRight = 0.5F * (left + right);
      sierpinski(vertices, depth - 1, left, leftRight, leftTop);
      sierpinski(vertices, depth - 1, leftRight, right, rightTop);
      sierpinski(vertices, depth - 1, leftTop, rightTop, top);
    }
  }

  std::unique_ptr<Model> Model::loadGameObjects(Device &device, int i, float scale) {
    (void)scale;
    std::vector<Model::Vertex> vertices{};
    sierpinski(vertices, i, {-0.5F, 0.5F}, {0.5F, 0.5F}, {0.0F, -0.5F});
    return std::make_unique<Model>(device, vertices);
  }

  std::vector<Model::Vertex> Model::updateGameObjects(int i, float scale) {
    (void)scale;
    std::vector<Model::Vertex> vertices{};
    sierpinski(vertices, i, {-0.5F, 0.5F}, {0.5F, 0.5F}, {0.0F, -0.5F});
    return vertices;
  }

  void Model::updateVertexBuffer(const std::vector<Vertex> &vertices) {
    vertexCount_ = static_cast<uint32_t>(vertices.size());
    assert(vertexCount_ >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;
    device_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer_, stagingBufferMemory_);

    void *data;
    vkMapMemory(device_.getDevice(), stagingBufferMemory_, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(device_.getDevice(), stagingBufferMemory_);
    auto *test = vertexBuffer_;
    auto *test3 = vertexBufferMemory_;
    device_.createBuffer(bufferSize,
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer_, vertexBufferMemory_);

    device_.copyBuffer(stagingBuffer_, vertexBuffer_, bufferSize);
    vkDestroyBuffer(device_.getDevice(), stagingBuffer_, nullptr);
    vkFreeMemory(device_.getDevice(), stagingBufferMemory_, nullptr);
    vkDestroyBuffer(device_.getDevice(), test, nullptr);
    vkFreeMemory(device_.getDevice(), test3, nullptr);
  }
}  // namespace ve