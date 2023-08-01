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
#include "utils.hpp"
#include "vulkan/vulkan_core.h"
namespace ve {

  class TextureRenderSystem {
    struct TexturePushConstantData {
      glm::vec4 color{1.0, 1.0, 1.0, 1.0};
      glm::vec2 offset{0.0, 0.0};
      unsigned int index;
    };

  public:
    struct Vertex {
      glm::vec2 pos;
      glm::vec2 texCoord;
      glm::vec4 color;

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
    TextureRenderSystem(Device &device, VkRenderPass renderPass,
                        VkDescriptorSetLayout globalSetLayout,
                        TextureRenderSystem::Builder &builder, TextureIndex textureIndex);
    TextureRenderSystem(const TextureRenderSystem &src) = delete;
    TextureRenderSystem &operator=(const TextureRenderSystem &rhs) = delete;
    ~TextureRenderSystem();

    void render(FrameInfo &info);
    void setIndexTexture(TextureIndex index) {
      if (index == TextureIndex::F0 || index == TextureIndex::F1 || index == TextureIndex::F2) {
        setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
      }
      textureIndex_ = index;
    }

    void setBuilderCoordinate(playerCoordinate *playerCoordinate) {
      float x = glm::cos(glm::radians(playerCoordinate->Angle));
      float y = glm::sin(glm::radians(playerCoordinate->Angle));

      if (x == -1.0F) {
        offset_.x = offset_.x - 0.05F;
        playerCoordinate->x = playerCoordinate->x - 0.05F;
      } else if (y == -1.0F) {
        offset_.y = offset_.y + 0.05F;
        playerCoordinate->y = playerCoordinate->y + 0.05F;
      } else if (x == 1.0F) {
        offset_.x = offset_.x + 0.05F;
        playerCoordinate->x = playerCoordinate->x + 0.05F;
      } else if (y == 1.0F) {
        offset_.y = offset_.y - 0.05F;
        playerCoordinate->y = playerCoordinate->y - 0.05F;
      }
    }

    void setColor(glm::vec4 color) { color_ = color; }

    TextureIndex getIndexTexture() const { return textureIndex_; }
    glm::vec4 getColor() const { return color_; }
    bool isInside(double x, double y) {
      return x > builder_.vertices[0].pos.x && x < builder_.vertices[1].pos.x
             && y > builder_.vertices[0].pos.y && y < builder_.vertices[2].pos.y;
    }

  private:
    void createPipelineLayout(VkDescriptorSetLayout *globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    std::unique_ptr<Buffer> vertexBuffer_;
    uint32_t vertexCount_;

    Device &device_;
    Builder builder_;
    glm::vec4 color_{};
    glm::vec2 offset_{};

    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    bool hasIndexBuffer_ = false;
    std::unique_ptr<Buffer> indexBuffer_;
    uint32_t indexCount_;

    TextureIndex textureIndex_;
  };
}  // namespace ve