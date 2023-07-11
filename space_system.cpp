#include "space_system.hpp"

#include <_types/_uint32_t.h>

#include <glm/gtc/constants.hpp>

#include "device.hpp"
#include "frame_info.hpp"
#include "vulkan/vulkan_core.h"
namespace ve {

  SpaceSystem::SpaceSystem(Device& device, VkRenderPass renderPass,
                           VkDescriptorSetLayout globalSetLayout)
      : device_(device) {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
  }

  SpaceSystem::~SpaceSystem() {
    vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr);
  }

  void SpaceSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(device_.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout");
    }
  }

  void SpaceSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/space.vert.spv",
                                           "shaders/space.frag.spv", pipelineConfig);
  }

  void SpaceSystem::render(FrameInfo& frameInfo) {
    pipeline_->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout_, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);
    vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
  }

}  // namespace ve