#include "simple_render_system.hpp"

#include <glm/gtc/constants.hpp>

#include "device.hpp"
#include "frame_info.hpp"
#include "vulkan/vulkan_core.h"
namespace ve {

  struct SimplePushConstantData {
    glm::mat4 transform{1.F};
    glm::mat4 normalMatrix{1.0F};
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

  void SimpleRenderSystem::renderGameObjects(FrameInfo& frameInfo,
                                             std::vector<GameObject>& gameObjects) {
    pipeline_->bind(frameInfo.commandBuffer);

    auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

    for (auto& obj : gameObjects) {
      SimplePushConstantData push{};
      auto modelMatrix = obj.transform.mat4();

      push.transform = projectionView * modelMatrix;
      push.normalMatrix = obj.transform.normalMatrix();

      vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout_,
                         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                         sizeof(SimplePushConstantData), &push);

      obj.model->bind(frameInfo.commandBuffer);
      obj.model->draw(frameInfo.commandBuffer);
    }
  }

}  // namespace ve