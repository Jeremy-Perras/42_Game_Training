#include <fstream>
#include <vector>

#include "device.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {
  class Pipeline {
  public:
    Pipeline(Device &device, VkExtent2D swapChainExtent, VkRenderPass renderPass);
    ~Pipeline();
    Pipeline(Pipeline const &rhs) = delete;
    Pipeline &operator=(Pipeline const &src) = delete;
    void bind(VkCommandBuffer commandBuffer);

  private:
    Device &_device;
    VkExtent2D _swapChainExtent;
    VkPipelineLayout _pipelineLayout;
    void createGraphicsPipeline();
    static std::vector<char> readFile(const std::string &filename);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    VkRenderPass _renderPass;
    VkPipeline _graphicsPipeline;
  };

}  // namespace ve