#pragma once

#include "app.hpp"
#include "texture.hpp"

namespace ve {

  class InterfaceModel {
  public:
    InterfaceModel(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorLayout);
    InterfaceModel(const InterfaceModel &src) = delete;
    InterfaceModel &operator=(const InterfaceModel &rhs) = delete;
    ~InterfaceModel();

    static std::vector<RenderSystem::Vertex> createSquareModel();
    void loadTexture(std::vector<std::shared_ptr<Texture>> *texture);
    void createMenuInterface(std::vector<GameObject> *menuInterface);
    void createPlayerInterface(std::vector<std::vector<GameObject>> *playerInterface,
                               std::vector<GameObject> *menuInterface, interfaceSize size);
    void createGameInterface(std::vector<std::string> map, std::vector<GameObject> *gameInterface);
    // Getter
    playerCoordinate getStartCoordinate() { return playerStart_; }
    GameObject *getPlayerPointer() { return playerPointer_; }

  private:
    Device &device_;
    VkRenderPass renderPass_;
    VkDescriptorSetLayout descriptorLayout_;
    int countStar_ = 0;
    playerCoordinate playerStart_;
    GameObject *playerPointer_;
  };

}  // namespace ve