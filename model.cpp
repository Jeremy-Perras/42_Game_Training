#include "model.hpp"

#include <cstddef>
#include <cstring>

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
    assert(vertexCount_ >= 3 && "Vertex cont must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;
    device_.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         vertexBuffer_, vertexBufferMemory_);
    void *data;
    vkMapMemory(device_.getDevice(), vertexBufferMemory_, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), bufferSize);
    vkUnmapMemory(device_.getDevice(), vertexBufferMemory_);
  }

  void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer_};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
  }

  void Model::draw(VkCommandBuffer commandBuffer) {
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
    attributeDescriptions[1].offset = offsetof(Vertex, color);
    return attributeDescriptions;
  }

  std::unique_ptr<Model> Model::createCircleModel(Device &device, unsigned int numSides) {
    std::vector<Model::Vertex> uniqueVertices{};
    for (int i = 0; i < static_cast<int>(numSides); i++) {
      float angle = static_cast<float>(i) * glm::two_pi<float>() / static_cast<float>(numSides);
      uniqueVertices.push_back({{glm::cos(angle), glm::sin(angle)}, {1.0F, 0.0F, 0.0F}});
    }
    uniqueVertices.push_back({});  // adds center vertex at 0, 0

    std::vector<Model::Vertex> vertices{};
    for (int i = 0; i < static_cast<int>(numSides); i++) {
      vertices.push_back(uniqueVertices[i]);
      vertices.push_back(uniqueVertices[(i + 1) % numSides]);
      vertices.push_back(uniqueVertices[numSides]);
    }
    return std::make_unique<Model>(device, vertices);
  }
}  // namespace ve