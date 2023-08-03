#pragma once

#include <utility>

#include "app.hpp"
#include "texture.hpp"

namespace ve {

  class InterfaceModel {
  public:
    InterfaceModel(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorLayout,
                   std::string &lvlPath);
    InterfaceModel(const InterfaceModel &src) = delete;
    InterfaceModel &operator=(const InterfaceModel &rhs) = delete;
    ~InterfaceModel();

    static std::vector<RenderSystem::Vertex> createSquareModel();
    void loadTexture(std::vector<std::shared_ptr<Texture>> *texture);
    void createMenuInterface(std::vector<GameObject> *menuInterface);
    void createPlayerInterface(std::vector<std::vector<GameObject>> *playerInterface,
                               std::vector<GameObject> *menuInterface, interfaceSize size);
    void createGameInterface(std::vector<std::string> map, std::vector<GameObject> *gameInterface);
    void saveInitialState(std::vector<GameObject> &gameInterface);
    void resetToInitialState(std::vector<GameObject> *gameInterface);

    void createGameMap(std::vector<GameObject> *gameInterface);
    static bool isInside(float x, float y, float i, float j) {
      (void)y;
      (void)j;

      return (i * 0.05 - 1.0F > x && x < (i + 1) * 0.05 - 1.0F) && j * 0.05 - 1.0F > y
             && y < (j + 1) * 0.05 - 1.0F;
    }

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
    std::vector<std::pair<TextureIndex, glm::vec4>> initialState_;
    float xStart_;
    float yStart_;
    int lineSize_;
    int coloneSize_;
    std::vector<std::string> map_;
  };

}  // namespace ve