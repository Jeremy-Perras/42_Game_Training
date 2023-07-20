#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <stdexcept>

#include "device.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {

  class MenuSystem {
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
               const MenuSystem::Builder &builder, id_t objId);
    MenuSystem(const MenuSystem &src) = delete;
    MenuSystem &operator=(const MenuSystem &rhs) = delete;
    ~MenuSystem();

    void render(FrameInfo &info);
    id_t getId() const { return id_; }

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);
    VkImageView createImageView(VkImage image, VkFormat format);
    void createTextureSampler();
    void createTextureImage();
    void createTextureImageView();

    std::unique_ptr<Buffer> vertexBuffer_;
    uint32_t vertexCount_;

    Device &device_;

    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    bool hasIndexBuffer_ = false;
    std::unique_ptr<Buffer> indexBuffer_;
    uint32_t indexCount_;

    id_t id_;
  };
}  // namespace ve