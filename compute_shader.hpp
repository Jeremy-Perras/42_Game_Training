#pragma once

#include <glm/glm.hpp>

#include "game_object.hpp"
#include "renderer.hpp"
#include "swap_chain.hpp"
namespace ve {
  struct UniformBufferObject {
    float deltaTime = 1.0F;
  };
  struct SimplePushConstantData {
    unsigned int index;
  };

  class ComputeShader {
  public:
    struct Particle {
      glm::vec2 position;
      glm::vec2 velocity;
      glm::vec4 color;
      // getters
      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    const uint32_t PARTICLE_COUNT = 8192;
    void createComputePipeline();
    ComputeShader(Device &device, VkRenderPass renderPass, Renderer &renderer);
    ComputeShader(const ComputeShader &src) = delete;
    ComputeShader &operator=(const ComputeShader &rhs) = delete;
    ~ComputeShader();

    void bind(VkCommandBuffer commandBuffer, int currentFrame);
    void draw(VkCommandBuffer commandBuffer) const;
    void createVertexBuffer(const std::vector<Particle> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);
    void createComputeShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);
    void createShaderStorageBuffers();
    void render(FrameInfo &frameInfo, std::vector<GameObject> &menuInterface,
                std::vector<std::vector<GameObject>> &playerInterface,
                std::vector<GameObject> &gameInterface, std::vector<GameObject> &displayInterface,std::vector<GameObject> &timeInterface);
    void createDescriptorPool();
    void draw(uint32_t currentFrame);
    void recordComputeCommandBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame);
    void createCommandPool();

    void createSyncObjects();
    // Getter
    VkDescriptorBufferInfo getBufferInfo(int i) {
      VkDescriptorBufferInfo uniformBufferInfo{};
      uniformBufferInfo.buffer = uniformBuffers_[i];
      uniformBufferInfo.offset = 0;
      uniformBufferInfo.range = sizeof(UniformBufferObject);
      return uniformBufferInfo;
    }

    VkDescriptorBufferInfo getInfoLastFrame(int i) {
      VkDescriptorBufferInfo storageBufferInfoLastFrame{};
      storageBufferInfoLastFrame.buffer
          = shaderStorageBuffers_[(i - 1) % SwapChain::MAX_FRAMES_IN_FLIGHT];
      storageBufferInfoLastFrame.offset = 0;
      storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;
      return storageBufferInfoLastFrame;
    }

    VkDescriptorBufferInfo getstorageInfoCurrentFrame(int i) {
      VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
      storageBufferInfoCurrentFrame.buffer = shaderStorageBuffers_[i];
      storageBufferInfoCurrentFrame.offset = 0;
      storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;
      return storageBufferInfoCurrentFrame;
    }
    void createPipelineLayout();
    void createComputeDescriptorSetLayout();
    void createComputeCommandBuffers();

  private:
    void createPipeline(VkRenderPass renderPass);
    void createUniformBuffers();
    void updateUniformBuffer(uint32_t currentImage);
    void createComputeDescriptorSets();
    void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

    double lastTime_ = 0.0F;
    double lastFrameTime_ = 0.0F;

    Device &device_;
    Renderer &renderer_;
    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    VkPipeline computePipeline_;
    VkPipelineLayout computePipelineLayout_;

    VkDescriptorSetLayout computeDescriptorSetLayout_;

    std::vector<VkBuffer> shaderStorageBuffers_;
    std::vector<VkDeviceMemory> shaderStorageBuffersMemory_;

    std::vector<VkBuffer> uniformBuffers_;
    std::vector<VkDeviceMemory> uniformBuffersMemory_;

    VkDescriptorPool computeDescriptorPool_;
    std::vector<VkDescriptorSet> computeDescriptorSets_;

    std::vector<void *> uniformBuffersMapped_;
    std::vector<VkCommandBuffer> computeCommandBuffers_;
  };
}  // namespace ve