#include "exit_render_system.hpp"

#include <iostream>

#include "../gamestructure/texture.hpp"
#include "../gamestructure/utils.hpp"

namespace ve {

  ExitRenderSystem::ExitRenderSystem(Device& device, Renderer& renderer,
                                     std::vector<std::shared_ptr<Texture>>& texture,
                                     ExitRenderSystem::Builder& builder, ExitIndex exitIndex)
      : device_(device),
        renderer_(renderer),
        builder_(builder),
        exitIndex_(exitIndex),
        texture_(texture) {
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

  ExitRenderSystem::~ExitRenderSystem() {
    vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr);
  }

  void ExitRenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ExitPushConstantData);

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

  void ExitRenderSystem::createPipeline() {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");
    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    Pipeline::enableAlphaBlending(pipelineConfig);

    pipelineConfig.attributeDescriptions = ExitRenderSystem::Vertex::getAttributeDescriptions();
    pipelineConfig.bindingDescriptions = ExitRenderSystem::Vertex::getBindingDescriptions();

    pipelineConfig.renderPass = renderer_.getSwapChainRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/exit.vert.spv",
                                           "shaders/exit.frag.spv", pipelineConfig);
  }

  std::vector<VkVertexInputBindingDescription> ExitRenderSystem::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
  }

  std::vector<VkVertexInputAttributeDescription>
  ExitRenderSystem::Vertex::getAttributeDescriptions() {
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

  void ExitRenderSystem::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer_->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    if (hasIndexBuffer_) {
      vkCmdBindIndexBuffer(commandBuffer, indexBuffer_->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
  }

  void ExitRenderSystem::draw(FrameInfo& frameInfo) const {
    if (hasIndexBuffer_) {
      vkCmdDrawIndexed(frameInfo.commandBuffer, indexCount_, 1, 0, 0, 0);
    } else {
      vkCmdDraw(frameInfo.commandBuffer, vertexCount_, 1, 0, 0);
    }
  }

  void ExitRenderSystem::createVertexBuffer(const std::vector<Vertex>& vertices) {
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

  void ExitRenderSystem::createIndexBuffers(const std::vector<uint32_t>& indices) {
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

  void ExitRenderSystem::createWriteDescriptorSet() {
    if (exitIndex_ < texture_.size()) {
      imageInfo_ = texture_[exitIndex_]->getImageInfo();
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

  void ExitRenderSystem::updateImageInfo() {
    if (exitIndex_ < texture_.size()) {
      imageInfo_ = texture_[exitIndex_]->getImageInfo();
    } else {
      imageInfo_ = texture_[0]->getImageInfo();
    }

    writeDescriptorSets_[0].pImageInfo = &imageInfo_;
  }

  void ExitRenderSystem::render(FrameInfo& frameInfo) {
    pipeline_->bind(frameInfo.commandBuffer);
    ExitPushConstantData push{};
    push.index = this->exitIndex_;
    push.offset = glm::vec4(offset_.x, offset_.y, 0.0, 0.0);
    push.color = this->color_;
    push.offset = glm::vec4(offset_.x, offset_.y, 0.0, 0.0);
    updateImageInfo();
    vkCmdPushDescriptorSetKHR_(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                               pipelineLayout_, 0, 1, writeDescriptorSets_.data());

    vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout_,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(ExitPushConstantData), &push);

    ExitRenderSystem::bind(frameInfo.commandBuffer);
    ExitRenderSystem::draw(frameInfo);
  }

  void ExitRenderSystem::homeState(std::string& press) {
    if (exitIndex_ == ExitIndex::HOME) {
      exitIndex_ = ExitIndex::FIRSTLEVEL;
      press.clear();
    }
  }

  void ExitRenderSystem::logicExitGame(std::string& press, GameState& gameState) {
    switch (exitIndex_) {
      case ExitIndex::FIRSTLEVEL:
        if (press == "Move the barrel") {
          exitIndex_ = ExitIndex::SECONDLEVELA;
          press.clear();
        } else if (press == "Sit down next to my friend") {
          exitIndex_ = ExitIndex::SECONDLEVELB;
          press.clear();
        }
        break;
      case ExitIndex::SECONDLEVELA:
        if (press == "Enter tunnel") {
          exitIndex_ = ExitIndex::THIRDLEVELA;
          press.clear();
        }
        break;
      case ExitIndex::THIRDLEVELA:
        if (press == "Read note") {
          exitIndex_ = ExitIndex::FOURTHLEVELA;
          press.clear();
        }
        break;
      case ExitIndex::FOURTHLEVELA:
        if (press == "Leave") {
          exitIndex_ = ExitIndex::FIFTHLEVELA;
          press.clear();
        }
        break;
      case ExitIndex::FIFTHLEVELA:
        if (press == "Look") {
          exitIndex_ = ExitIndex::SIXTHLEVELA;
          press.clear();
        }
        break;
      case ExitIndex::SIXTHLEVELA:
        if (press == "Get on the boat") {
          exitIndex_ = ExitIndex::FINALLEVELA;
          press.clear();
        }
        break;
      case ExitIndex::FINALLEVELA:
        if (press == "Yes") {
          exitIndex_ = ExitIndex::HOME;
          press.clear();
        }
        break;
      case ExitIndex::SECONDLEVELB:
        if (press == "Light a match") {
          exitIndex_ = ExitIndex::THIRDLEVELB;
          press.clear();
        }
        break;
      case ExitIndex::THIRDLEVELB:
        if (press == "Stay") {
          exitIndex_ = ExitIndex::FINALLEVELB;
          press.clear();
        }
        break;
      case FINALLEVELB:
        if (press == "Y") {
          exitIndex_ = ExitIndex::HELLOFRIEND;
          press.clear();
        }
        std::cout << "Exit to final level b" << '\n';
        helloFriendTiming_ = std::chrono::high_resolution_clock::now();
        song_.playMrRobot();
        break;
      case ExitIndex::HELLOFRIEND:
        if (std::chrono::duration<float, std::chrono::seconds::period>(
                std::chrono::high_resolution_clock::now() - helloFriendTiming_)
                .count()
            > 1000.F) {
          song_.StopMrRobot();
          gameState = GameState::MENU;
          press.clear();
        }
        break;
      case HOME:
        break;
    }
  }

}  // namespace ve