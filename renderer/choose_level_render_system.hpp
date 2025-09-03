#pragma once

#include <glm/glm.hpp>

#include "../system/buffer.hpp"
#include "../system/frame_info.hpp"
#include "../system/pipeline.hpp"
#include "../system/renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  class ChooseLevel {
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

    ChooseLevel(Device &device, Renderer &renderer);
    ChooseLevel(const ChooseLevel &src) = delete;
    ChooseLevel &operator=(const ChooseLevel &rhs) = delete;
    ~ChooseLevel();

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;
    void renderGameObjects(FrameInfo &frameInfo);

  private:
    const std::vector<ChooseLevel::Vertex> vertices_;
    void createVertexBuffer(const std::vector<ChooseLevel::Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);
    void createPipelineLayout();
    void createPipeline();

    Device &device_;
    Renderer &renderer_;
    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    std::unique_ptr<Buffer> vertexBuffer_;
    Builder builder_ = {{{{-1.0F, -1.0F}}, {{1.0F, -1.0F}}, {{1.0F, 1.0F}}, {{-1.0F, 1.0F}}},
                        {0, 1, 2, 0, 2, 3}};
    bool hasIndexBuffer_ = false;
    std::unique_ptr<Buffer> indexBuffer_;
    uint32_t indexCount_;

    uint32_t vertexCount_;
  };
}  // namespace ve