#pragma once

#include "app.hpp"

namespace ve {

  struct interfaceSize {
    int f0;
    int f1;
    int f2;
  };
  class Model {
  public:
    Model(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorLayout);
    Model(const Model &src) = delete;
    Model &operator=(const Model &rhs) = delete;
    ~Model();

    static std::vector<RenderSystem::Vertex> createSquareModel();
    void loadTexture(std::vector<std::shared_ptr<Texture>> *texture);
    void menuInterface(std::vector<GameObject> *menuInterface);
    void playerInterface(std::vector<std::vector<GameObject>> *playerInterface,
                         std::vector<GameObject> *menuInterface, interfaceSize size);

  private:
    Device &device_;
    VkRenderPass renderPass_;
    VkDescriptorSetLayout descriptorLayout_;
  };

}  // namespace ve