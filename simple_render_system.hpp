#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <stdexcept>

#include "device.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  class SimpleRenderSystem {
  public:
    SimpleRenderSystem(Device &device, VkRenderPass renderPass,
                       VkDescriptorSetLayout globalSetLayout = nullptr);
    SimpleRenderSystem(const SimpleRenderSystem &src) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &rhs) = delete;
    ~SimpleRenderSystem();

    void renderGameObjects(FrameInfo &frameInfo, std::vector<GameObject> &gameObjects);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device &device_;

    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;
  };
}  // namespace ve