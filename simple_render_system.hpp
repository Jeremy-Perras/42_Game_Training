#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <stdexcept>

#include "device.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  class SimpleRenderSystem {
  public:
    SimpleRenderSystem(Device &device, VkRenderPass renderPass);
    SimpleRenderSystem(const SimpleRenderSystem &src) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &rhs) = delete;
    ~SimpleRenderSystem();

    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject> &gameObjects);

  private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    Device &device_;

    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;
  };
}  // namespace ve