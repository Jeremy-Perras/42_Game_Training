#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "../gamestructure/texture.hpp"
#include "../gamestructure/utils.hpp"
#include "../song/song.hpp"
#include "../system/buffer.hpp"
#include "../system/descriptors.hpp"
#include "../system/device.hpp"
#include "../system/frame_info.hpp"
#include "../system/pipeline.hpp"
#include "../system/renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  struct ExitPushConstantData {
    glm::vec4 color{1.0, 1.0, 1.0, 1.0};
    glm::vec4 offset{0.0, 0.0, 0.0, 0.0};
    unsigned int index;
  };

  class ExitRenderSystem {
  public:
    struct Vertex {
      glm::vec2 pos;
      glm::vec2 texCoord;
      // getters
      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct Builder {
      std::vector<Vertex> vertices;
      std::vector<uint32_t> indices;
    };

    void logicExitGame(std::string &press, GameState &gameState);

    ExitIndex getLogicIndex() { return exitIndex_; }

    ExitRenderSystem(Device &device, Renderer &renderer,
                     std::vector<std::shared_ptr<Texture>> &texture, Builder &builder,
                     ExitIndex exitIndex);
    ExitRenderSystem(const ExitRenderSystem &src) = delete;
    ExitRenderSystem &operator=(const ExitRenderSystem &rhs) = delete;
    ~ExitRenderSystem();

    void bind(VkCommandBuffer commandBuffer);
    void draw(FrameInfo &frameInfo) const;
    void createVertexBuffer(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);
    void homeState(std::string &press);

    void render(FrameInfo &info);

    void setColor(glm::vec4 color) { color_ = color; }

    glm::vec4 getColor() const { return color_; }

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
    ExitIndex exitIndex_;
    float playerCountOffsetX_ = 0;
    float playerCountOffsetY_ = 0;
    std::vector<std::shared_ptr<Texture>> &texture_;
    std::unique_ptr<DescriptorSetLayoutPush> textureDescriptorSetLayout_;
    std::array<VkWriteDescriptorSet, 1> writeDescriptorSets_{};
    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR_;
    VkDescriptorImageInfo imageInfo_{};

    std::chrono::steady_clock::time_point helloFriendTiming_;
    Song song_;
  };
}  // namespace ve