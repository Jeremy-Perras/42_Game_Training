#include "simple_render_system.hpp"

#include <glm/gtc/constants.hpp>

#include "device.hpp"
#include "pipeline.hpp"
#include "vulkan/vulkan_core.h"
namespace ve {

  struct SimplePushConstantData {
    int index;
  };

  SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass,
                                         VkDescriptorSetLayout globalSetLayout)
      : device_(device) {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
  }

  SimpleRenderSystem::~SimpleRenderSystem() {
    vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr);
  }

  void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
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

  void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    Pipeline::enableAlphaBlending(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipelineConfig.attributeDescriptions = Model::Vertex::getAttributeDescriptions();
    pipelineConfig.bindingDescriptions = Model::Vertex::getBindingDescriptions();

    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/shader.vert.spv",
                                           "shaders/shader.frag.spv", pipelineConfig);
  }

  void SimpleRenderSystem::renderGameObjects(FrameInfo& frameInfo, GameObject& gameObjects) {
    pipeline_->bind(frameInfo.commandBuffer);

    //   SimplePushConstantData push{};
    //   push.color = obj.color;

    // vkCmdPushConstants(commandBuffer, pipelineLayout_,
    //                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
    //                    sizeof(SimplePushConstantData), &push);
    // vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                         pipelineLayout_, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);
    gameObjects.model->bind(frameInfo.commandBuffer);
    gameObjects.model->draw(frameInfo.commandBuffer);
  }

}  // namespace ve