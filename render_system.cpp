#include "render_system.hpp"

#include <glm/gtc/constants.hpp>

#include "device.hpp"
#include "pipeline.hpp"
#include "vulkan/vulkan_core.h"
namespace ve {

  struct SimplePushConstantData {
    int index;
  };

  RenderSystem::RenderSystem(Device &device, VkRenderPass renderPass,
                             VkDescriptorSetLayout globalSetLayout,
                             const RenderSystem::Builder &builder)
      : device_(device) {
    {
      createPipelineLayout(globalSetLayout);
      createPipeline(renderPass);
      createVertexBuffer(builder.vertices);
      createIndexBuffers(builder.indices);
    }
  }

  RenderSystem::~RenderSystem() {
    vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr);
  }

  void RenderSystem::createVertexBuffer(const std::vector<Vertex> &vertices) {
    vertexCount_ = static_cast<uint32_t>(vertices.size());
    assert(vertexCount_ >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;
    uint32_t vertexSize = sizeof(vertices[0]);

    Buffer stagingBuffer{
        device_,
        vertexSize,
        vertexCount_,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)vertices.data());

    vertexBuffer_ = std::make_unique<Buffer>(
        device_, vertexSize, vertexCount_,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer_->getBuffer(), bufferSize);
  }

  void RenderSystem::createIndexBuffers(const std::vector<uint32_t> &indices) {
    indexCount_ = static_cast<uint32_t>(indices.size());
    hasIndexBuffer_ = indexCount_ > 0;

    if (!hasIndexBuffer_) {
      return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount_;
    uint32_t indexSize = sizeof(indices[0]);

    Buffer stagingBuffer{
        device_,
        indexSize,
        indexCount_,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)indices.data());

    indexBuffer_ = std::make_unique<Buffer>(
        device_, indexSize, indexCount_,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), indexBuffer_->getBuffer(), bufferSize);
  }

  void RenderSystem::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer_->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
  }

  void RenderSystem::draw(VkCommandBuffer commandBuffer) const {
    vkCmdDraw(commandBuffer, vertexCount_, 1, 0, 0);
  }

  std::vector<VkVertexInputBindingDescription> RenderSystem::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
  }

  std::vector<VkVertexInputAttributeDescription> RenderSystem::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, colors);

    return attributeDescriptions;
  }

  void RenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    (void)globalSetLayout;
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;

    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device_.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout");
    }
  }

  void RenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    Pipeline::enableAlphaBlending(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipelineConfig.attributeDescriptions = RenderSystem::Vertex::getAttributeDescriptions();
    pipelineConfig.bindingDescriptions = RenderSystem::Vertex::getBindingDescriptions();

    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/shader.vert.spv",
                                           "shaders/shader.frag.spv", pipelineConfig);
  }

  void RenderSystem::renderGameObjects(FrameInfo &frameInfo) {
    pipeline_->bind(frameInfo.commandBuffer);

    // SimplePushConstantData push{};
    // push.color = obj.color;

    // vkCmdPushConstants(commandBuffer, pipelineLayout_,
    //                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
    //                    sizeof(SimplePushConstantData), &push);
    // vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                         pipelineLayout_, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);
    RenderSystem::bind(frameInfo.commandBuffer);
    RenderSystem::draw(frameInfo.commandBuffer);
  }

}  // namespace ve