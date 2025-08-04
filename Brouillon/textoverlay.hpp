#pragma once
#include <iomanip>
#include <sstream>
#pragma once

#include <glm/glm.hpp>

#include "buffer.hpp"
#include "frame_info.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "texture/stb_font_consolas_24_latin1.inl"
#include "vulkan/vulkan_core.h"
#define TEXTOVERLAY_MAX_CHAR_COUNT 2048

namespace ve {
  class TextOverlay {
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

    TextOverlay(Device &device, Renderer &renderer, const TextOverlay::Builder &builder);
    TextOverlay(const TextOverlay &src) = delete;
    TextOverlay &operator=(const TextOverlay &rhs) = delete;
    ~TextOverlay();
    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer) const;
    void renderGameObjects(FrameInfo &frameInfo);

    enum TextAlign { alignLeft, alignCenter, alignRight };
    uint32_t numLetters;
    // Getter
  private:
    const std::vector<TextOverlay::Vertex> vertices_;
    void createVertexBuffer(const std::vector<TextOverlay::Vertex> &vertices);
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
    stb_fontchar stbFontData[STB_FONT_consolas_24_latin1_NUM_CHARS];
  };
}  // namespace ve