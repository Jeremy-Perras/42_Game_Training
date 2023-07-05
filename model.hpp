#pragma once

#include <_types/_uint32_t.h>

#include <cassert>
#include <glm/gtc/constants.hpp>
#include <map>

#include "device.hpp"
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
    Model(Device &device, const std::vector<Vertex> &vertices);
    Model(const Model &src) = delete;
    Model &operator=(const Model &rhs) = delete;
    ~Model();
    static std::unique_ptr<Model> createCircleModel(Device &device, unsigned int numSides);
    static std::map<std::pair<int, int>, std::pair<int, std::vector<Model::Vertex>>>
    createSquareModel(float scale);
    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;
    void changeColorModel(int indice);
    static void sierpinski(std::vector<Model::Vertex> &vertices, int depth, glm::vec2 left,
                           glm::vec2 right, glm::vec2 top);
    static std::unique_ptr<Model> loadGameObjects(Device &device, int i, float scale);
    void updateVertexBuffer(const std::vector<Vertex> &vertices);
    static std::vector<Model::Vertex> updateGameObjects(int i, float scale);

  private:
    const std::vector<Vertex> vertices_;
    void createVertexBuffer(const std::vector<Vertex> &vertices);
    Device &device_;
    VkBuffer vertexBuffer_;
    VkDeviceMemory vertexBufferMemory_;
    uint32_t vertexCount_;
    VkBuffer srcvertexBuffer_;
    VkDeviceMemory srcvertexBufferMemory_;
    uint32_t srcvertexCount_;
    VkBuffer stagingBuffer_;
    VkDeviceMemory stagingBufferMemory_;
  };

}  // namespace ve