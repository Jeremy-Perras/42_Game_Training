#pragma once

#include "app.hpp"

namespace ve {

  constexpr int textureSize = 10;
  class Model {
  public:
    Model(Device &device);
    Model(const Model &src) = delete;
    Model &operator=(const Model &rhs) = delete;
    ~Model();

    static std::vector<RenderSystem::Vertex> createSquareModel();
    void loadTexture(std::vector<std::shared_ptr<Texture>> *texture);
    void createMenu(std::vector<GameObject> *gameObjects, VkRenderPass renderPass,
                    VkDescriptorSetLayout descriptorLayout);

  private:
    Device &device_;
    std::vector<TextureRenderSystem::Builder> builder_;
  };

}  // namespace ve