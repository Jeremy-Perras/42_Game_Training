#pragma once

#include <glm/glm.hpp>

#include "buffer.hpp"
#include "frame_info.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  class RenderSystem {
  public:
    struct Vertex {
      glm::vec2 position;
      glm::vec3 colors;
      // getters
      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct Builder {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};
    };

    RenderSystem(Device &device, Renderer &renderer, VkDescriptorSetLayout globalSetLayout,
                 const RenderSystem::Builder &builder);
    RenderSystem(const RenderSystem &src) = delete;
    RenderSystem &operator=(const RenderSystem &rhs) = delete;
    ~RenderSystem();

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;
    void renderGameObjects(FrameInfo &frameInfo);

  private:
    const std::vector<RenderSystem::Vertex> vertices_;
    void createVertexBuffer(const std::vector<RenderSystem::Vertex> &vertices);
    VkImageView createImageView(VkImage image, VkFormat format);
    void createIndexBuffers(const std::vector<uint32_t> &indices);
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
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

    VkBuffer stagingBuffer_;
    VkDeviceMemory stagingBufferMemory_;
  };
}  // namespace ve