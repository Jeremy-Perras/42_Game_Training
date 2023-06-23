#pragma once
#include <_types/_uint32_t.h>

#include <fstream>
#include <string>
#include <vector>

#include "device.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {

  struct PipelineConfigInfo {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
  };

  class Pipeline {
  public:
    Pipeline(Device &device, const std::string &vertFilepath, const std::string &fragFilepath,
             const PipelineConfigInfo &configInfo);
    Pipeline(const Pipeline &src) = delete;
    Pipeline &operator=(const Pipeline &rhs) = delete;
    ~Pipeline();
    void bind(VkCommandBuffer commandBuffer);

    static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

  private:
    static std::vector<char> readFile(const std::string &filepath);
    void createGraphicPipeline(const std::string &vertFilepath, const std::string &fragFilepath,
                               const PipelineConfigInfo &configInfo);
    void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);
    Device &device_;
    VkPipeline graphicsPipeline_;
    VkShaderModule vertShaderModule_;
    VkShaderModule fragShaderModule_;
  };
}  // namespace ve