#include "simple_render_system.hpp"

#include <glm/gtc/constants.hpp>

#include "device.hpp"
#include "vulkan/vulkan_core.h"
namespace ve {

  struct SimplePushConstantData {
    glm::mat2 transform{1.0F};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
  };

  SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass)
      : device_(device) {
    createPipelineLayout();
    createPipeline(renderPass);
  }

  SimpleRenderSystem::~SimpleRenderSystem() {
    vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr);
  }

  void SimpleRenderSystem::createPipelineLayout() {
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
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/shader.vert.spv",
                                           "shaders/shader.frag.spv", pipelineConfig);
  }

  void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer,
                                             std::vector<GameObject>& gameObjects) {
    pipeline_->bind(commandBuffer);
    for (auto& obj : gameObjects) {
      obj.transform.rotation = glm::mod(obj.transform.rotation + 0.01F, glm::two_pi<float>());

      SimplePushConstantData push{};
      push.offset = obj.transform.translation;

      push.color = obj.color;
      push.transform = obj.transform.mat2();

      vkCmdPushConstants(commandBuffer, pipelineLayout_,
                         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                         sizeof(SimplePushConstantData), &push);

      obj.model->bind(commandBuffer);
      obj.model->draw(commandBuffer);
    }
  }

}  // namespace ve