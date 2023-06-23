#include "app.hpp"

#include <_types/_uint32_t.h>

#include <array>
#include <memory>
#include <mutex>
#include <stdexcept>

#include "pipeline.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  Application::Application() {
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
  }

  Application::~Application() {
    vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr);
  }

  void Application::mainLoop() {
    while (static_cast<int>(window_.shouldClose()) == 0) {
      glfwPollEvents();
      drawFrame();
    }
    vkDeviceWaitIdle(device_.getDevice());
  }
  void Application::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(device_.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout");
    }
  }
  void Application::createPipeline() {
    auto pipelineConfig
        = Pipeline::defaultPipelineConfigInfo(swapChain_.getWidth(), swapChain_.getHeight());
    pipelineConfig.renderPass = swapChain_.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/shader.vert.spv",
                                           "shaders/shader.frag.spv", pipelineConfig);
  }
  void Application::createCommandBuffers() {
    commandBuffers_.resize(swapChain_.getImageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device_.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());
    if (vkAllocateCommandBuffers(device_.getDevice(), &allocInfo, commandBuffers_.data())
        != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers");
    }
    for (int i = 0; i < static_cast<int>(commandBuffers_.size()); i++) {
      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      if (vkBeginCommandBuffer(commandBuffers_[i], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer");
      }

      VkRenderPassBeginInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = swapChain_.getRenderPass();
      renderPassInfo.framebuffer = swapChain_.getFrameBuffer(i);
      renderPassInfo.renderArea.offset = {0, 0};
      renderPassInfo.renderArea.extent = swapChain_.getSwapChainExtent();

      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {{0.1F, 0.1F, 0.1F, 1.0F}};
      clearValues[1].depthStencil = {1.0F, 0};
      renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      renderPassInfo.pClearValues = clearValues.data();

      vkCmdBeginRenderPass(commandBuffers_[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
      pipeline_->bind(commandBuffers_[i]);
      vkCmdDraw(commandBuffers_[i], 3, 1, 0, 0);
      vkCmdEndRenderPass(commandBuffers_[i]);

      if (vkEndCommandBuffer(commandBuffers_[i]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer");
      }
    }
  }
  void Application::drawFrame() {
    uint32_t imageIndex;
    auto result = swapChain_.acquireNextImage(&imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image");
    }

    result = swapChain_.submitCommandBuffers(&commandBuffers_[imageIndex], &imageIndex);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image");
    }
  }

}  // namespace ve