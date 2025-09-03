#include "star_nest_render_system.hpp"

namespace ve {

  StarNest::StarNest(Device &device, Renderer &renderer) : device_(device), renderer_(renderer) {
    createPipelineLayout();
    createPipeline();
    createVertexBuffer(builder_.vertices);
    createIndexBuffers(builder_.indices);
  }

  StarNest::~StarNest() { vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr); }

  void StarNest::createVertexBuffer(const std::vector<Vertex> &vertices) {
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

  void StarNest::createIndexBuffers(const std::vector<uint32_t> &indices) {
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

  void StarNest::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer_->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    if (hasIndexBuffer_) {
      vkCmdBindIndexBuffer(commandBuffer, indexBuffer_->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
  }

  void StarNest::draw(VkCommandBuffer commandBuffer) const {
    if (hasIndexBuffer_) {
      vkCmdDrawIndexed(commandBuffer, indexCount_, 1, 0, 0, 0);
    } else {
      vkCmdDraw(commandBuffer, vertexCount_, 1, 0, 0);
    }
  }

  std::vector<VkVertexInputBindingDescription> StarNest::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
  }

  std::vector<VkVertexInputAttributeDescription> StarNest::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    return attributeDescriptions;
  }

  void StarNest::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstants);

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

  void StarNest::createPipeline() {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    Pipeline::enableAlphaBlending(pipelineConfig);
    pipelineConfig.renderPass = renderer_.getSwapChainRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipelineConfig.attributeDescriptions = StarNest::Vertex::getAttributeDescriptions();
    pipelineConfig.bindingDescriptions = StarNest::Vertex::getBindingDescriptions();

    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/starNest.vert.spv",
                                           "shaders/starNest.frag.spv", pipelineConfig);
  }

  void StarNest::renderGameObjects(FrameInfo &frameInfo) {
    pipeline_->bind(frameInfo.commandBuffer);
    auto now = std::chrono::system_clock::now();

    PushConstants pushConstants;

    pushConstants.iResolution
        = glm::vec3(renderer_.getSwapChain().getSwapChainExtent().width,
                    renderer_.getSwapChain().getSwapChainExtent().height, 1.0F);

    pushConstants.iTime = static_cast<float>(glfwGetTime());

    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout_, VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(PushConstants), &pushConstants);

    StarNest::bind(frameInfo.commandBuffer);
    StarNest::draw(frameInfo.commandBuffer);
  }

}  // namespace ve