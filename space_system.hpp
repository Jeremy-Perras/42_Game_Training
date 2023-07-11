#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <stdexcept>

#include "camera.hpp"
#include "device.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  class SpaceSystem {
  public:
    SpaceSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    SpaceSystem(const SpaceSystem &src) = delete;
    SpaceSystem &operator=(const SpaceSystem &rhs) = delete;
    ~SpaceSystem();

    void render(FrameInfo &info);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device &device_;

    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;
  };
}  // namespace ve