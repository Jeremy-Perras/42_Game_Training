#include "model.hpp"

#include <_types/_uint32_t.h>
#include <sys/_types/_size_t.h>

#include "utils.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <cstddef>
#include <cstring>
#include <memory>
#include <stdexcept>

#include "device.hpp"
#include "libs/tiny_obj_loader.h"
#include "vulkan/vulkan_core.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <unordered_map>

namespace std {
  template <> struct hash<ve::Model::Vertex> {
    size_t operator()(ve::Model::Vertex const &vertex) const {
      size_t seed = 0;

      ve::hashCombine(seed, vertex.position, vertex.colors, vertex.normal, vertex.uv);
      return seed;
    }
  };
}  // namespace std
namespace ve {

  Model::Model(Device &device, const Model::Builder &builder) : device_(device) {
    createVertexBuffer(builder.vertices);
    createIndexBuffer(builder.indices);
  }
  Model::~Model() {
    vkDestroyBuffer(device_.getDevice(), vertexBuffer_, nullptr);
    vkFreeMemory(device_.getDevice(), vertexBufferMemory_, nullptr);
    if (hasIndexBuffer_) {
      vkDestroyBuffer(device_.getDevice(), indexBuffer_, nullptr);
      vkFreeMemory(device_.getDevice(), indexBufferMemory_, nullptr);
    }
  }

  std::unique_ptr<Model> Model::createModelFromFile(Device &device, const std::string &filepath) {
    Builder builder{};
    builder.loadModel(filepath);
    return std::make_unique<Model>(device, builder);
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

  void Model::createIndexBuffer(const std::vector<uint32_t> &indices) {
    indexCount_ = static_cast<uint32_t>(indices.size());
    hasIndexBuffer_ = indexCount_ > 0;
    if (!hasIndexBuffer_) {
      return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount_;

    device_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer_, stagingBufferMemory_);
    void *data;
    vkMapMemory(device_.getDevice(), stagingBufferMemory_, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(device_.getDevice(), stagingBufferMemory_);

    device_.createBuffer(bufferSize,
                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer_, indexBufferMemory_);

    device_.copyBuffer(stagingBuffer_, indexBuffer_, bufferSize);
    vkDestroyBuffer(device_.getDevice(), stagingBuffer_, nullptr);
    vkFreeMemory(device_.getDevice(), stagingBufferMemory_, nullptr);
  }

  void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer_};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    if (hasIndexBuffer_) {
      vkCmdBindIndexBuffer(commandBuffer, indexBuffer_, 0, VK_INDEX_TYPE_UINT32);
    }
  }

  void Model::draw(VkCommandBuffer commandBuffer) const {
    if (hasIndexBuffer_) {
      vkCmdDrawIndexed(commandBuffer, indexCount_, 1, 0, 0, 0);
    } else {
      vkCmdDraw(commandBuffer, vertexCount_, 1, 0, 0);
    }
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
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, colors);
    return attributeDescriptions;
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
  void Model::Builder::loadModel(const std::string &filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
      throw std::runtime_error(warn + err);
    }
    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (const auto &shape : shapes) {
      for (const auto &index : shape.mesh.indices) {
        Vertex vertex{};

        if (index.vertex_index >= 0) {
          vertex.position = {
              attrib.vertices[3 * index.vertex_index + 0],
              attrib.vertices[3 * index.vertex_index + 1],
              attrib.vertices[3 * index.vertex_index + 2],
          };

          auto colorIndex = 3 * index.vertex_index + 2;
          if (colorIndex < static_cast<int>(attrib.colors.size())) {
            vertex.colors = {
                attrib.colors[colorIndex - 2],
                attrib.colors[colorIndex - 1],
                attrib.colors[colorIndex - 0],
            };
          } else {
            vertex.colors = {1.F, 1.F, 1.F};  // set default color
          }
        }

        if (index.normal_index >= 0) {
          vertex.normal = {
              attrib.normals[3 * index.normal_index + 0],
              attrib.normals[3 * index.normal_index + 1],
              attrib.normals[3 * index.normal_index + 2],
          };
        }

        if (index.texcoord_index >= 0) {
          vertex.uv = {
              attrib.texcoords[2 * index.texcoord_index + 0],
              attrib.texcoords[2 * index.texcoord_index + 1],
          };
        }
        if (uniqueVertices.count(vertex) == 0) {
          uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
          vertices.push_back(vertex);
        }
        indices.push_back(uniqueVertices[vertex]);
      }
    }
  }
}  // namespace ve
