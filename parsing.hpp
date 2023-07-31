#pragma once
#include <string>

#include "game_object.hpp"
namespace ve {
  class Parsing {
  public:
    Parsing(const std::string &filepath, Device &device, VkRenderPass renderPass,
            VkDescriptorSetLayout descriptorLayout);
    Parsing(const Parsing &src) = delete;
    Parsing &operator=(const Parsing &rhs) = delete;
    ~Parsing(){};

    void initParsing(const std::string &filepath);
    void createMap(std::vector<GameObject> *gameInterface);
    // Getter
  private:
    std::vector<std::string> map_;
    Device &device_;
    VkRenderPass renderPass_;
    VkDescriptorSetLayout descriptorLayout_;
  };
}  // namespace ve