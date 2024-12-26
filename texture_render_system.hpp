#pragma once

#include <glm/glm.hpp>

#include "buffer.hpp"
#include "descriptors.hpp"
#include "frame_info.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "texture.hpp"
#include "utils.hpp"
namespace ve {

  class TextureRenderSystem {
    struct TexturePushConstantData {
      glm::vec4 color{1.0, 1.0, 1.0, 1.0};
      glm::vec4 offset{0.0, 0.0, 0.0, 0.0};
      unsigned int index;
    };

  public:
    struct Vertex {
      glm::vec2 pos;
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
    void draw(FrameInfo &frameInfo) const;
    void createVertexBuffer(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);
    TextureRenderSystem(Device &device, Renderer &renderer,
                        std::vector<std::shared_ptr<Texture>> &texture,
                        TextureRenderSystem::Builder &builder, TextureIndex textureIndex);
    TextureRenderSystem(const TextureRenderSystem &src) = delete;
    TextureRenderSystem &operator=(const TextureRenderSystem &rhs) = delete;
    ~TextureRenderSystem();

    void render(FrameInfo &info);
    void setIndexTexture(TextureIndex index) {
      if (index == TextureIndex::DISCARD) {
        textureIndex_ = index;
        color_.w = 0.0;
      } else {
        textureIndex_ = index;
      }
    }

    void setBuilderCoordinate(playerCoordinate *playerCoordinate) {
      float x = glm::cos(glm::radians(playerCoordinate->Angle));
      float y = glm::sin(glm::radians(playerCoordinate->Angle));
      if (x == -1.0F) {
        offset_.x = offset_.x - 0.05F;
        playerCoordinate->x = playerCoordinate->x - 0.05F;
        playerCountOffsetX_--;
      } else if (y == -1.0F) {
        offset_.y = offset_.y + 0.05F;
        playerCoordinate->y = playerCoordinate->y + 0.05F;
        playerCountOffsetY_++;
      } else if (x == 1.0F) {
        offset_.x = offset_.x + 0.05F;
        playerCoordinate->x = playerCoordinate->x + 0.05F;
        playerCountOffsetX_++;
      } else if (y == 1.0F) {
        offset_.y = offset_.y - 0.05F;
        playerCoordinate->y = playerCoordinate->y - 0.05F;
        playerCountOffsetY_--;
      }
    }

    void setPlayerTextureOrientation(playerCoordinate playerCoordinate) {
      float angle = playerCoordinate.Angle;
      if (angle == 0.0) {
        textureIndex_ = TextureIndex::PLAYERRIGHT;

      } else if (angle == 90.0 || angle == -270.0) {
        textureIndex_ = TextureIndex::PLAYERUP;
      } else if (angle == 180.0 || angle == -180.0) {
        textureIndex_ = TextureIndex::PLAYERLEFT;

      } else if (angle == 270.0 || angle == -90) {
        textureIndex_ = TextureIndex::PLAYERDOWN;
      }
    }

    void setPushTranslationCoordinate(float translation, bool x, bool y) {
      if (x) {
        offset_.x += translation;
      } else if (y) {
        offset_.y += translation;
      }
    }

    void resetPushCoordinate() {
      offset_.x -= 0.05F * playerCountOffsetX_;
      offset_.y -= 0.05F * playerCountOffsetY_;
      playerCountOffsetX_ = 0;
      playerCountOffsetY_ = 0;
    }

    void setColor(glm::vec4 color) { color_ = color; }

    TextureIndex getIndexTexture() const { return textureIndex_; }
    glm::vec4 getColor() const { return color_; }

    bool isInside(double x, double y) {
      return x > (builder_.vertices[0].pos.x) && (x < builder_.vertices[1].pos.x)
             && y > (builder_.vertices[0].pos.y) && (y < builder_.vertices[2].pos.y);
    }

  private:
    void createPipelineLayout();
    void createPipeline();
    void createUniformBuffers();
    void createWriteDescriptorSet();
    void updateImageInfo();
    std::unique_ptr<Buffer> vertexBuffer_;
    uint32_t vertexCount_;

    Device &device_;
    Renderer &renderer_;
    Builder builder_;
    glm::vec4 color_{};
    glm::vec2 offset_{0.0F, 0.0F};

    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    bool hasIndexBuffer_ = false;
    std::unique_ptr<Buffer> indexBuffer_;
    uint32_t indexCount_;

    TextureIndex textureIndex_;
    float playerCountOffsetX_ = 0;
    float playerCountOffsetY_ = 0;

    std::vector<std::shared_ptr<Texture>> &texture_;

    std::unique_ptr<DescriptorSetLayoutPush> textureDescriptorSetLayout_;

    std::array<VkWriteDescriptorSet, 1> writeDescriptorSets_{};
    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR_;
    VkDescriptorImageInfo imageInfo_{};
  };
}  // namespace ve