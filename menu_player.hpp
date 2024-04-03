#pragma once

#include <glm/glm.hpp>

#include "buffer.hpp"
#include "descriptors.hpp"
#include "device.hpp"
#include "frame_info.hpp"
#include "pipeline.hpp"
#include "render_system.hpp"
#include "renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  class MenuPlayer {
  public:
    MenuPlayer(Device &device, Renderer &renderer);
    MenuPlayer(const MenuPlayer &src) = delete;
    MenuPlayer &operator=(const MenuPlayer &rhs) = delete;
    ~MenuPlayer();

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;
    void render(FrameInfo &frameInfo);

  private:
    const std::vector<RenderSystem::Vertex> vertices_;
    void createVertexBuffer(const std::vector<RenderSystem::Vertex> &vertices);
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

    VkBuffer stagingBuffer_;
    VkDeviceMemory stagingBufferMemory_;

    std::vector<void *> uniformBuffersMapped_;
    std::vector<VkBuffer> uniformBuffers_;
    std::vector<VkDeviceMemory> uniformBuffersMemory_;
  };
}  // namespace ve