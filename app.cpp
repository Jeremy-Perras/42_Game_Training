#include "app.hpp"

#include <glm/gtc/constants.hpp>
namespace ve {

  struct SimplePushConstantData {
    glm::mat2 transform{1.0F};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
  };

  Application::Application() : m_fpscount_(0) {
    gettimeofday(&start_, NULL);
    gettimeofday(&end_, NULL);
    loadGameObjects();
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
  }

  Application::~Application() {
    vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr);
  }

  void Application::mainLoop() {
    while (static_cast<int>(window_.shouldClose()) == 0) {
      m_fpscount_++;
      if (getElapsedTime(end_, start_) >= 1000) {
        window_.updateFrame(m_fpscount_);
        m_fpscount_ = 0;
        gettimeofday(&start_, NULL);
      }
      glfwPollEvents();
      drawFrame();
      gettimeofday(&end_, NULL);
    }
    vkDeviceWaitIdle(device_.getDevice());
  }

  void Application::loadGameObjects() {
    std::vector<Model::Vertex> vertices{{{0.0F, -0.5F}, {1.0F, 0.0F, 0.0F}},
                                        {{0.5F, 0.5F}, {0.0F, 1.0F, 0.0F}},
                                        {{-0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}}};
    auto model_ = std::make_shared<Model>(device_, vertices);
    auto triangle = GameObject::createGameObject();

    triangle.model = model_;
    triangle.color = {0.1F, 0.8F, 0.1F};
    triangle.transform2d.translation.x = 0.2F;
    triangle.transform2d.scale = {2.0F, 0.5F};
    triangle.transform2d.rotation = 0.25F * glm ::two_pi<float>();

    gameObjects_.push_back(std::move(triangle));
  }

  void Application::createPipelineLayout() {
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

  void Application::createPipeline() {
    assert(swapChain_ != nullptr && "Cannot create pipeline before swap chain");
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = swapChain_->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/shader.vert.spv",
                                           "shaders/shader.frag.spv", pipelineConfig);
  }

  void Application::recreateSwapChain() {
    auto extent = window_.getExtent();
    while (extent.height == 0 || extent.width == 0) {
      extent = window_.getExtent();
      glfwWaitEvents();
    }
    vkDeviceWaitIdle(device_.getDevice());
    if (swapChain_ == nullptr) {
      swapChain_ = std::make_unique<SwapChain>(device_, extent);
    } else {
      swapChain_ = std::make_unique<SwapChain>(device_, extent, std::move(swapChain_));
      if (swapChain_->getImageCount() != commandBuffers_.size()) {
        freeCommandBuffers();
        createCommandBuffers();
      }
    }
    createPipeline();
  }

  void Application::freeCommandBuffers() {
    vkFreeCommandBuffers(device_.getDevice(), device_.getCommandPool(), commandBuffers_.size(),
                         commandBuffers_.data());
    commandBuffers_.clear();
  }

  void Application::createCommandBuffers() {
    commandBuffers_.resize(swapChain_->getImageCount());

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

  void Application::recordCommandBuffer(int imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(commandBuffers_[imageIndex], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain_->getRenderPass();
    renderPassInfo.framebuffer = swapChain_->getFrameBuffer(imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain_->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.01F, 0.01F, 0.01F, 1.0F}};
    clearValues[1].depthStencil = {1.0F, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers_[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0F;
    viewport.y = 0.0F;
    viewport.width = static_cast<float>(swapChain_->getSwapChainExtent().width);
    viewport.height = static_cast<float>(swapChain_->getSwapChainExtent().height);
    viewport.minDepth = 0.0F;
    viewport.maxDepth = 1.0F;
    VkRect2D scissor{{0, 0}, swapChain_->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffers_[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers_[imageIndex], 0, 1, &scissor);

    renderGameObjects(commandBuffers_[imageIndex]);

    vkCmdEndRenderPass(commandBuffers_[imageIndex]);

    if (vkEndCommandBuffer(commandBuffers_[imageIndex]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer");
    }
  }

  void Application::renderGameObjects(VkCommandBuffer commandBuffer) {
    pipeline_->bind(commandBuffer);
    for (auto& obj : gameObjects_) {
      obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01F, glm::two_pi<float>());

      SimplePushConstantData push{};
      push.offset = obj.transform2d.translation;

      push.color = obj.color;
      push.transform = obj.transform2d.mat2();

      vkCmdPushConstants(commandBuffer, pipelineLayout_,
                         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                         sizeof(SimplePushConstantData), &push);

      obj.model->bind(commandBuffer);
      obj.model->draw(commandBuffer);
    }
  }

  void Application::drawFrame() {
    uint32_t imageIndex;
    auto result = swapChain_->acquireNextImage(&imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image");
    }
    recordCommandBuffer(static_cast<int>(imageIndex));

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR
        || window_.wasWindowResized()) {
      window_.resetWindowResizedFlag();
      recreateSwapChain();
      return;
    }

    result = swapChain_->submitCommandBuffers(&commandBuffers_[imageIndex], &imageIndex);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image");
    }
  }

  long double Application::getElapsedTime(struct timeval end, struct timeval begin) {
    long double elapsed;
    elapsed = static_cast<long double>(end.tv_sec - begin.tv_sec) * 1000.0
              + static_cast<long double>(end.tv_usec - begin.tv_usec) / 1000.0;
    return (elapsed);
  }

}  // namespace ve