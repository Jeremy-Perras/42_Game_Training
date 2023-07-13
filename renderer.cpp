#include "renderer.hpp"

#include <glm/gtc/constants.hpp>
#include <memory>
#include <stdexcept>

#include "swap_chain.hpp"
namespace ve {

  Renderer::Renderer(Window &window, Device &device) : window_(window), device_(device) {
    recreateSwapChain();
    createCommandBuffers();
  }

  Renderer::~Renderer() { freeCommandBuffers(); }

  void Renderer::recreateSwapChain() {
    auto extent = window_.getExtent();
    while (extent.height == 0 || extent.width == 0) {
      extent = window_.getExtent();
      glfwWaitEvents();
    }
    vkDeviceWaitIdle(device_.getDevice());
    if (swapChain_ == nullptr) {
      swapChain_ = std::make_unique<SwapChain>(device_, extent);
    } else {
      std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain_);
      swapChain_ = std::make_unique<SwapChain>(device_, extent, std::move(swapChain_));
      if (!oldSwapChain->compareSwapFormat(*swapChain_)) {
        throw std::runtime_error("Swap chain or depth format has changed");
      }
      swapChain_->createImageViews();
    }
  }

  void Renderer::freeCommandBuffers() {
    vkFreeCommandBuffers(device_.getDevice(), device_.getCommandPool(), commandBuffers_.size(),
                         commandBuffers_.data());
    commandBuffers_.clear();
  }

  void Renderer::createCommandBuffers() {
    commandBuffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device_.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());
    if (vkAllocateCommandBuffers(device_.getDevice(), &allocInfo, commandBuffers_.data())
        != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers");
    }
  }

  VkCommandBuffer Renderer::beginFrame() {
    assert(!isFrameStarted_ && "Can't call beginFrame while already in progress");

    auto result = swapChain_->acquireNextImage(&currentImageIndex_);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameStarted_ = true;

    auto *commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
  }

  void Renderer::endFrame() {
    assert(isFrameStarted_ && "Can't call endFrame while frame is not in progress");
    auto *commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer");
    }
    auto result = swapChain_->submitCommandBuffers(&commandBuffer, &currentImageIndex_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR
        || window_.wasWindowResized()) {
      window_.resetWindowResizedFlag();
      recreateSwapChain();

    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image");
    }
    isFrameStarted_ = false;
    currentFrameIndex_ = (currentFrameIndex_ + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
  }

  void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted_ && "Can't call beginSwapChainRenderPass while frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer()
           && "Can't beging render pass ont command buffer from a fifferent frame");
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain_->getRenderPass();
    renderPassInfo.framebuffer = swapChain_->getFrameBuffer(static_cast<int>(currentImageIndex_));
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain_->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0F, 0.0F, 0.0F, 0.0F}};
    clearValues[1].depthStencil = {1.0F, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0F;
    viewport.y = 0.0F;
    viewport.width = static_cast<float>(swapChain_->getSwapChainExtent().width);
    viewport.height = static_cast<float>(swapChain_->getSwapChainExtent().height);
    viewport.minDepth = 0.0F;
    viewport.maxDepth = 1.0F;
    VkRect2D scissor{{0, 0}, swapChain_->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }

  void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted_ && "Can't call endSwapChainRenderPass while frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer()
           && "Can't end render pass ont command buffer from a fifferent frame");
    vkCmdEndRenderPass(commandBuffer);
  }

}  // namespace ve