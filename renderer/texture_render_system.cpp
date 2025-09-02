#include "texture_render_system.hpp"

namespace ve {

  TextureRenderSystem::TextureRenderSystem(Device& device, Renderer& renderer,
                                           std::vector<std::shared_ptr<Texture>>& texture,
                                           TextureRenderSystem::Builder& builder,
                                           TextureIndex textureIndex)
      : device_(device),
        renderer_(renderer),
        builder_(builder),
        textureIndex_(textureIndex),
        texture_(texture) {
    if (textureIndex_ == TextureIndex::RED) {
      color_ = glm::vec4(1.0, 0.0, 0.0, 1.0);
    } else if (textureIndex_ == TextureIndex::GREEN) {
      color_ = glm::vec4(0.0, 1.0, 0.0, 1.0);
    } else if (textureIndex_ == TextureIndex::BLUE) {
      color_ = glm::vec4(0.0, 0.0, 1.0, 1.0);
    } else {
      color_ = glm::vec4(1.0, 1.0, 1.0, 1.0);
    }

    textureDescriptorSetLayout_ = DescriptorSetLayoutPush::Builder(device_)
                                      .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                  VK_SHADER_STAGE_FRAGMENT_BIT)
                                      .build();
    createPipelineLayout();
    createPipeline();
    createVertexBuffer(builder.vertices);
    createIndexBuffers(builder.indices);

    vkCmdPushDescriptorSetKHR_ = device_.getPushCommand();
    createWriteDescriptorSet();
  }

  TextureRenderSystem::~TextureRenderSystem() {
    vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr);
  }

  void TextureRenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(TexturePushConstantData);

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

  void TextureRenderSystem::createPipeline() {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");
    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    Pipeline::enableAlphaBlending(pipelineConfig);

    pipelineConfig.attributeDescriptions = TextureRenderSystem::Vertex::getAttributeDescriptions();
    pipelineConfig.bindingDescriptions = TextureRenderSystem::Vertex::getBindingDescriptions();

    pipelineConfig.renderPass = renderer_.getSwapChainRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/texture.vert.spv",
                                           "shaders/texture.frag.spv", pipelineConfig);
  }

  std::vector<VkVertexInputBindingDescription>
  TextureRenderSystem::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
  }

  std::vector<VkVertexInputAttributeDescription>
  TextureRenderSystem::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
  }

  void TextureRenderSystem::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer_->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    if (hasIndexBuffer_) {
      vkCmdBindIndexBuffer(commandBuffer, indexBuffer_->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
  }

  void TextureRenderSystem::draw(FrameInfo& frameInfo) const {
    if (hasIndexBuffer_) {
      vkCmdDrawIndexed(frameInfo.commandBuffer, indexCount_, 1, 0, 0, 0);
    } else {
      vkCmdDraw(frameInfo.commandBuffer, vertexCount_, 1, 0, 0);
    }
  }

  void TextureRenderSystem::createVertexBuffer(const std::vector<Vertex>& vertices) {
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
    stagingBuffer.writeToBuffer((void*)vertices.data());

    vertexBuffer_ = std::make_unique<Buffer>(
        device_, vertexSize, vertexCount_,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer_->getBuffer(), bufferSize);
  }

  void TextureRenderSystem::createIndexBuffers(const std::vector<uint32_t>& indices) {
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
    stagingBuffer.writeToBuffer((void*)indices.data());

    indexBuffer_ = std::make_unique<Buffer>(
        device_, indexSize, indexCount_,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), indexBuffer_->getBuffer(), bufferSize);
  }

  void TextureRenderSystem::createWriteDescriptorSet() {
    if (textureIndex_ < texture_.size()) {
      imageInfo_ = texture_[textureIndex_]->getImageInfo();
    } else {
      imageInfo_ = texture_[0]->getImageInfo();
    }

    writeDescriptorSets_[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets_[0].dstSet = 0;
    writeDescriptorSets_[0].dstBinding = 0;
    writeDescriptorSets_[0].descriptorCount = 1;
    writeDescriptorSets_[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets_[0].pImageInfo = &imageInfo_;
  }

  void TextureRenderSystem::updateImageInfo() {
    if (textureIndex_ < texture_.size()) {
      imageInfo_ = texture_[textureIndex_]->getImageInfo();
    } else {
      imageInfo_ = texture_[0]->getImageInfo();
    }

    writeDescriptorSets_[0].pImageInfo = &imageInfo_;
  }

  void TextureRenderSystem::render(FrameInfo& frameInfo) {
    pipeline_->bind(frameInfo.commandBuffer);
    TexturePushConstantData push{};
    push.index = this->textureIndex_;
    push.offset = glm::vec4(offset_.x, offset_.y, 0.0, 0.0);
    push.color = this->color_;
    push.offset = glm::vec4(offset_.x, offset_.y, 0.0, 0.0);

    updateImageInfo();
    vkCmdPushDescriptorSetKHR_(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                               pipelineLayout_, 0, 1, writeDescriptorSets_.data());

    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout_,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(TexturePushConstantData), &push);

    TextureRenderSystem::bind(frameInfo.commandBuffer);
    TextureRenderSystem::draw(frameInfo);
  }

}  // namespace ve