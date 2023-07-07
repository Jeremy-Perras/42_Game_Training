#pragma once

#include <_types/_uint32_t.h>

#include <cassert>
#include <glm/gtc/constants.hpp>
#include <map>
#include <memory>

#include "device.hpp"
#include "vulkan/vulkan_core.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace ve {
  class Model {
  public:
    struct Vertex {
      glm::vec3 position;
      glm::vec3 colors;
      glm::vec3 normal;
      glm::vec2 uv{};

      // getters
      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

      bool operator==(const Vertex &other) const {
        return position == other.position && colors == other.colors && normal == other.normal
               && uv == other.uv;
      }
    };

    struct Builder {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};
      void loadModel(const std::string &filepath);
    };

    Model(Device &device, const Model::Builder &builder);
    Model(const Model &src) = delete;
    Model &operator=(const Model &rhs) = delete;
    ~Model();

    static std::unique_ptr<Model> createModelFromFile(Device &device, const std::string &filepath);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;
    void changeColorModel(int indice);

    static std::unique_ptr<Model> loadGameObjects(Device &device, int i, float scale);
    void updateVertexBuffer(const std::vector<Vertex> &vertices);

  private:
    const std::vector<Vertex> vertices_;
    void createVertexBuffer(const std::vector<Vertex> &vertices);
    void createIndexBuffer(const std::vector<uint32_t> &indices);
    Device &device_;

    VkBuffer vertexBuffer_;
    VkDeviceMemory vertexBufferMemory_;
    uint32_t vertexCount_;

    bool hasIndexBuffer_ = false;

    VkBuffer indexBuffer_;
    VkDeviceMemory indexBufferMemory_;
    uint32_t indexCount_;

    VkBuffer srcvertexBuffer_;
    VkDeviceMemory srcvertexBufferMemory_;
    uint32_t srcvertexCount_;
    VkBuffer stagingBuffer_;
    VkDeviceMemory stagingBufferMemory_;
  };

}  // namespace ve