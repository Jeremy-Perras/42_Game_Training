#include "render_system.hpp"

namespace ve {

  struct UboScene {
    float i = 0.5;
  } uboScene;

  RenderSystem::RenderSystem(Device &device, Renderer &renderer,
                             VkDescriptorSetLayout globalSetLayout,
                             const RenderSystem::Builder &builder)
      : device_(device), renderer_(renderer) {
    textureDescriptorSetLayout_
        = DescriptorSetLayoutPush::Builder(device_)
              .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
              .build();

    createUniformBuffers();
    createPipelineLayout(globalSetLayout);
    createPipeline();
    createVertexBuffer(builder.vertices);
    createIndexBuffers(builder.indices);
  }

  void RenderSystem::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UboScene);

    uniformBuffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
      device_.createBuffer(
          bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
          uniformBuffers_[i], uniformBuffersMemory_[i]);

      vkMapMemory(device_.getDevice(), uniformBuffersMemory_[i], 0, bufferSize, 0,
                  &uniformBuffersMapped_[i]);
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
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
        textureDescriptorSetLayout_->getDescriptorSetLayout()};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device_.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout");
    }
  }

  void RenderSystem::createPipeline() {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    Pipeline::enableAlphaBlending(pipelineConfig);
    pipelineConfig.renderPass = renderer_.getSwapChainRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipelineConfig.attributeDescriptions = RenderSystem::Vertex::getAttributeDescriptions();
    pipelineConfig.bindingDescriptions = RenderSystem::Vertex::getBindingDescriptions();

    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/shader.vert.spv",
                                           "shaders/shader.frag.spv", pipelineConfig);
  }

  void RenderSystem::updateUniformBuffer(uint32_t currentImage) {
    UboScene ubo{};
    ubo.i = 1.0;

    memcpy(uniformBuffersMapped_[currentImage], &ubo, sizeof(ubo));
  }

  void RenderSystem::renderGameObjects(FrameInfo &frameInfo) {
    pipeline_->bind(frameInfo.commandBuffer);

    VkDescriptorBufferInfo uniformBufferInfo{};
    uniformBufferInfo.buffer = uniformBuffers_[frameInfo.frameIndex];
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = sizeof(UboScene);

    std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].dstSet = 0;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSets[0].pBufferInfo = &uniformBufferInfo;
    PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR = device_.getPushCommand();
    updateUniformBuffer(frameInfo.frameIndex);
    vkCmdPushDescriptorSetKHR(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipelineLayout_, 0, 1, writeDescriptorSets.data());

    RenderSystem::bind(frameInfo.commandBuffer);
    RenderSystem::draw(frameInfo.commandBuffer);
  }

}  // namespace ve