#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <glm/glm.hpp>
#include <stdexcept>

#include "buffer.hpp"
#include "device.hpp"
#include "frame_info.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "texture.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {

  class MenuSystem {
    struct SimplePushConstantData {
      unsigned int index;
    };

  public:
    struct Vertex {
      glm::vec2 pos;
      glm::vec3 color;
      glm::vec2 texCoord;

      // getters
      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct Builder {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};
    };

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;
    void createVertexBuffer(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);
    MenuSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout,
               const MenuSystem::Builder &builder, int textureIndex);
    MenuSystem(const MenuSystem &src) = delete;
    MenuSystem &operator=(const MenuSystem &rhs) = delete;
    ~MenuSystem();

    void render(FrameInfo &info);
    void setId(int id) { textureIndex_ = id; }

    bool isInside(double x, double y) {
      return x > builder_.vertices[0].pos.x && x < builder_.vertices[1].pos.x
             && y > builder_.vertices[0].pos.y && y < builder_.vertices[2].pos.y;
    }

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    std::unique_ptr<Buffer> vertexBuffer_;
    uint32_t vertexCount_;

    Device &device_;
    Builder builder_;

    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    bool hasIndexBuffer_ = false;
    std::unique_ptr<Buffer> indexBuffer_;
    uint32_t indexCount_;

    int textureIndex_;
  };
}  // namespace ve