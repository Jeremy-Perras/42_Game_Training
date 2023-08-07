#include "compute_shader.hpp"

#include <random>

namespace ve {

  ComputeShader::ComputeShader(Device& device, VkRenderPass renderPass, Renderer& renderer)
      : device_(device), renderer_(renderer) {
    createComputeDescriptorSetLayout();
    createPipelineLayout();
    createPipeline(renderPass);
    createComputePipeline();
    createShaderStorageBuffers();
    createUniformBuffers();
    createDescriptorPool();
    createComputeDescriptorSets();
    createComputeCommandBuffers();
  }

  ComputeShader::~ComputeShader() {
    vkDestroyPipeline(device_.getDevice(), computePipeline_, nullptr);
    vkDestroyPipelineLayout(device_.getDevice(), computePipelineLayout_, nullptr);
    vkDestroyPipelineLayout(device_.getDevice(), pipelineLayout_, nullptr);
    vkDestroyDescriptorPool(device_.getDevice(), computeDescriptorPool_, nullptr);
    vkDestroyDescriptorSetLayout(device_.getDevice(), computeDescriptorSetLayout_, nullptr);
    for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroyBuffer(device_.getDevice(), shaderStorageBuffers_[i], nullptr);
      vkFreeMemory(device_.getDevice(), shaderStorageBuffersMemory_[i], nullptr);
    }
    for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroyBuffer(device_.getDevice(), uniformBuffers_[i], nullptr);
      vkFreeMemory(device_.getDevice(), uniformBuffersMemory_[i], nullptr);
    }
  }

  void ComputeShader::createPipelineLayout() {
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

  void ComputeShader::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultComputePipelineConfigInfo(pipelineConfig);

    pipelineConfig.attributeDescriptions = ComputeShader::Particle::getAttributeDescriptions();
    pipelineConfig.bindingDescriptions = ComputeShader::Particle::getBindingDescriptions();

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<Pipeline>(device_, "shaders/compute_shader.vert.spv",
                                           "shaders/compute_shader.frag.spv", pipelineConfig);
  }

  std::vector<VkVertexInputBindingDescription> ComputeShader::Particle::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Particle);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
  }

  std::vector<VkVertexInputAttributeDescription>
  ComputeShader::Particle::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Particle, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Particle, color);

    return attributeDescriptions;
  }

  void ComputeShader::createComputePipeline() {
    auto computeShaderCode = Pipeline::readFile("shaders/comp.spv");
    VkShaderModule computeShaderModule;

    createShaderModule(computeShaderCode, &computeShaderModule);

    VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main";

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout_;

    if (vkCreatePipelineLayout(device_.getDevice(), &pipelineLayoutInfo, nullptr,
                               &computePipelineLayout_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create compute pipeline layout!");
    }

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = computePipelineLayout_;
    pipelineInfo.stage = computeShaderStageInfo;

    if (vkCreateComputePipelines(device_.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                 &computePipeline_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create compute pipeline!");
    }

    vkDestroyShaderModule(device_.getDevice(), computeShaderModule, nullptr);
  }

  void ComputeShader::createShaderStorageBuffers() {
    // Initialize particles
    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0F, 1.0F);
    // Initial particle positions on a circle
    std::vector<Particle> particles(PARTICLE_COUNT);
    for (auto& particle : particles) {
      float r = 0.25F * sqrt(rndDist(rndEngine));
      float theta = rndDist(rndEngine) * 2.0F * 3.14159265358979323846F;
      float x = r * cos(theta) * HEIGHT / WIDTH;
      float y = r * sin(theta);
      particle.position = glm::vec2(x, y);
      particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025F;
      particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0F);
    }

    uint32_t vertexCount_ = static_cast<uint32_t>(particles.size());
    uint32_t vertexSize = sizeof(particles[0]);
    VkDeviceSize bufferSize = sizeof(Particle) * particles.size();

    // Create a staging buffer used to upload data to the gpu
    Buffer stagingBuffer{
        device_,
        vertexSize,
        vertexCount_,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };
    stagingBuffer.map();
    stagingBuffer.writeToBuffer(particles.data());

    shaderStorageBuffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    shaderStorageBuffersMemory_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    // Copy initial particle data to all storage buffers
    for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
      device_.createBuffer(bufferSize,
                           VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
                               | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shaderStorageBuffers_[i],
                           shaderStorageBuffersMemory_[i]);
      device_.copyBuffer(stagingBuffer.getBuffer(), shaderStorageBuffers_[i], bufferSize);
    }
  }

  void ComputeShader::bind(VkCommandBuffer commandBuffer, int currentFrame) {
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &shaderStorageBuffers_[currentFrame], offsets);
  }

  void ComputeShader::draw(VkCommandBuffer commandBuffer) const {
    vkCmdDraw(commandBuffer, PARTICLE_COUNT, 1, 0, 0);
  }

  void ComputeShader::render(FrameInfo& frameInfo, std::vector<GameObject>& menuInterface,
                             std::vector<std::vector<GameObject>>& playerInterface,
                             std::vector<GameObject>& gameInterface,
                             std::vector<GameObject>& displayInterface) {
    size_t currentFrame = frameInfo.frameIndex;
    renderer_.computeWait();
    updateUniformBuffer(currentFrame);
    renderer_.computeResetFences();
    recordComputeCommandBuffer(computeCommandBuffers_[currentFrame], currentFrame);
    renderer_.computeQueueSubmit(&(computeCommandBuffers_[currentFrame]));

    auto* commandBuffer = renderer_.beginFrame(true);
    frameInfo.commandBuffer = commandBuffer;
    renderer_.beginSwapChainRenderPass(commandBuffer);

    for (const auto& obj : gameInterface) {
      if (obj.textureRenderSystem
          && obj.textureRenderSystem->getIndexTexture() != TextureIndex::LOST) {
        obj.textureRenderSystem->render(frameInfo);
      }
    }

    for (const auto& obj : displayInterface) {
      if (obj.textureRenderSystem) {
        obj.textureRenderSystem->render(frameInfo);
      }
    }

    for (const auto& ob : playerInterface) {
      for (const auto& obj : ob) {
        if (obj.textureRenderSystem) {
          obj.textureRenderSystem->render(frameInfo);
        } else {
          obj.renderSystem->renderGameObjects(frameInfo);
        }
      }
    }

    for (auto& obj : menuInterface) {
      if (obj.textureRenderSystem) {
        obj.textureRenderSystem->render(frameInfo);
      } else {
        obj.renderSystem->renderGameObjects(frameInfo);
      }
    }

    pipeline_->bind(commandBuffer);

    ComputeShader::bind(commandBuffer, static_cast<int>(currentFrame));
    ComputeShader::draw(commandBuffer);

    renderer_.endSwapChainRenderPass(commandBuffer);
    renderer_.endFrame(true);

    double currentTime = glfwGetTime() - frameInfo.Time;
    lastFrameTime_ = (currentTime - lastTime_) * 1000.0;
    lastTime_ = currentTime;
  }

  void ComputeShader::createShaderModule(const std::vector<char>& code,
                                         VkShaderModule* shaderModule) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    if (vkCreateShaderModule(device_.getDevice(), &createInfo, nullptr, shaderModule)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create shader module!");
    }
  }

  void ComputeShader::createComputeDescriptorSetLayout() {
    std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].pImmutableSamplers = nullptr;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorCount = 1;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings[1].pImmutableSamplers = nullptr;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    layoutBindings[2].binding = 2;
    layoutBindings[2].descriptorCount = 1;
    layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings[2].pImmutableSamplers = nullptr;
    layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 3;
    layoutInfo.pBindings = layoutBindings.data();

    if (vkCreateDescriptorSetLayout(device_.getDevice(), &layoutInfo, nullptr,
                                    &computeDescriptorSetLayout_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create compute descriptor set layout!");
    }
  }

  void ComputeShader::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT);

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT) * 2;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device_.getDevice(), &poolInfo, nullptr, &computeDescriptorPool_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor pool!");
    }
  }

  void ComputeShader::createComputeDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(SwapChain::MAX_FRAMES_IN_FLIGHT,
                                               computeDescriptorSetLayout_);
    VkDescriptorSetAllocateInfo allocInfo{};

    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = computeDescriptorPool_;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    computeDescriptorSets_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device_.getDevice(), &allocInfo, computeDescriptorSets_.data())
        != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
      VkDescriptorBufferInfo uniformBufferInfo{};
      uniformBufferInfo.buffer = uniformBuffers_[i];
      uniformBufferInfo.offset = 0;
      uniformBufferInfo.range = sizeof(UniformBufferObject);

      std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
      descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[0].dstSet = computeDescriptorSets_[i];
      descriptorWrites[0].dstBinding = 0;
      descriptorWrites[0].dstArrayElement = 0;
      descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrites[0].descriptorCount = 1;
      descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

      VkDescriptorBufferInfo storageBufferInfoLastFrame{};
      storageBufferInfoLastFrame.buffer
          = shaderStorageBuffers_[(i - 1) % SwapChain::MAX_FRAMES_IN_FLIGHT];
      storageBufferInfoLastFrame.offset = 0;
      storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

      descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[1].dstSet = computeDescriptorSets_[i];
      descriptorWrites[1].dstBinding = 1;
      descriptorWrites[1].dstArrayElement = 0;
      descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      descriptorWrites[1].descriptorCount = 1;
      descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

      VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
      storageBufferInfoCurrentFrame.buffer = shaderStorageBuffers_[i];
      storageBufferInfoCurrentFrame.offset = 0;
      storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

      descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[2].dstSet = computeDescriptorSets_[i];
      descriptorWrites[2].dstBinding = 2;
      descriptorWrites[2].dstArrayElement = 0;
      descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      descriptorWrites[2].descriptorCount = 1;
      descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;

      vkUpdateDescriptorSets(device_.getDevice(), 3, descriptorWrites.data(), 0, nullptr);
    }
  }

  void ComputeShader::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
      device_.createBuffer(
          bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
          uniformBuffers_[i], uniformBuffersMemory_[i]);

      vkMapMemory(device_.getDevice(), uniformBuffersMemory_[i], 0, bufferSize, 0,
                  &uniformBuffersMapped_[i]);
    }
  }

  void ComputeShader::updateUniformBuffer(uint32_t currentImage) {
    UniformBufferObject ubo{};
    ubo.deltaTime = static_cast<float>(lastFrameTime_) * 2.0F;

    memcpy(uniformBuffersMapped_[currentImage], &ubo, sizeof(ubo));
  }

  void ComputeShader::recordComputeCommandBuffer(VkCommandBuffer commandBuffer,
                                                 uint32_t currentFrame) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording compute command buffer!");
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline_);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout_,
                            0, 1, &computeDescriptorSets_[currentFrame], 0, nullptr);

    vkCmdDispatch(commandBuffer, PARTICLE_COUNT / 256, 1, 1);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record compute command buffer!");
    }
  }

  void ComputeShader::createComputeCommandBuffers() {
    computeCommandBuffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = device_.getCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)computeCommandBuffers_.size();

    if (vkAllocateCommandBuffers(device_.getDevice(), &allocInfo, computeCommandBuffers_.data())
        != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate compute command buffers!");
    }
  }

}  // namespace ve