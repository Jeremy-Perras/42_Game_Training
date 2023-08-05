#pragma once

#include <utility>

#include "game_object.hpp"
#include "texture.hpp"

namespace ve {

  class InterfaceModel {
  public:
    InterfaceModel(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorLayout,
                   std::string &lvlPath, std::vector<std::shared_ptr<Texture>> &texture,
                   std::vector<GameObject> &menuInterface,
                   std::vector<std::vector<GameObject>> &playerInterface,
                   std::vector<GameObject> &gameInterface);

    InterfaceModel(const InterfaceModel &src) = delete;
    InterfaceModel &operator=(const InterfaceModel &rhs) = delete;
    ~InterfaceModel();

    static std::vector<RenderSystem::Vertex> createSquareModel();
    void loadTexture();
    void createMenuInterface();
    void createPlayerInterface();
    void createGameInterface(std::vector<std::string> map);
    void saveInitialState();
    void resetToInitialState();

    void createGameMap();
    static bool isInside(float x, float y, float i, float j) {
      return (i * 0.05 - 1.0F > x && x < (i + 1) * 0.05 - 1.0F) && j * 0.05 - 1.0F > y
             && y < (j + 1) * 0.05 - 1.0F;
    }

    // Getter
    playerCoordinate getStartCoordinate() { return playerStart_; }
    GameObject *getPlayerPointer() { return playerPointer_; }
    int getCountStart() const { return countStar_; }

  private:
    void chooseOrientationPlayer(char c, TextureRenderSystem::Builder builder);

    Device &device_;
    VkRenderPass renderPass_;
    std::vector<GameObject> &gameInterface_;
    std::vector<GameObject> &menuInterface_;
    std::vector<std::vector<GameObject>> &playerInterface_;
    std::vector<std::shared_ptr<Texture>> &texture_;

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