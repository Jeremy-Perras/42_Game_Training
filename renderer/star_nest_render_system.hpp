#pragma once

#include <glm/glm.hpp>

#include "../system/buffer.hpp"
#include "../system/frame_info.hpp"
#include "../system/pipeline.hpp"
#include "../system/renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  class StarNest {
  public:
    struct Vertex {
      glm::vec2 pos;
      // getters
      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct PushConstants {  // struct for push constants
      glm::vec3 iResolution;
      float iTime;
    };

    struct Builder {
      std::vector<Vertex> vertices;
      std::vector<uint32_t> indices;
    };

    StarNest(Device &device, Renderer &renderer, const StarNest::Builder &builder);
    StarNest(const StarNest &src) = delete;
    StarNest &operator=(const StarNest &rhs) = delete;
    ~StarNest();

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;
    void renderGameObjects(FrameInfo &frameInfo);

  private:
    const std::vector<StarNest::Vertex> vertices_;
    void createVertexBuffer(const std::vector<StarNest::Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);
    void createPipelineLayout();
    void createPipeline();

    Device &device_;
    Renderer &renderer_;
    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    std::unique_ptr<Buffer> vertexBuffer_;

    bool hasIndexBuffer_ = false;
    std::unique_ptr<Buffer> indexBuffer_;
    uint32_t indexCount_;

    uint32_t vertexCount_;
  };
}  // namespace ve