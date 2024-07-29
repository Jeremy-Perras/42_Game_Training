#pragma once

#include <glm/glm.hpp>

#include "buffer.hpp"
#include "descriptors.hpp"
#include "frame_info.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  class ShaderRenderSystem {
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
      glm::vec2 iMouse;
    };

    struct Builder {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};
    };

    ShaderRenderSystem(Device &device, Renderer &renderer,
                       const ShaderRenderSystem::Builder &builder);
    ShaderRenderSystem(const ShaderRenderSystem &src) = delete;
    ShaderRenderSystem &operator=(const ShaderRenderSystem &rhs) = delete;
    ~ShaderRenderSystem();

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;
    void renderGameObjects(FrameInfo &frameInfo);

  private:
    const std::vector<ShaderRenderSystem::Vertex> vertices_;
    void createVertexBuffer(const std::vector<ShaderRenderSystem::Vertex> &vertices);
    VkImageView createImageView(VkImage image, VkFormat format);
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