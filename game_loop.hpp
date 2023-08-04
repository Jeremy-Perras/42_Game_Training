#pragma once
#include "descriptors.hpp"
#include "interface_model.hpp"
#include "renderer.hpp"
namespace ve {
  class GameLoop {
  public:
    GameLoop(Device &device, Renderer &renderer);
    GameLoop(const GameLoop &src) = delete;
    GameLoop &operator=(const GameLoop &rhs) = delete;
    ~GameLoop();

    void gameInit();
    void createDescriptor();
    void textureInit();
    void gameLoop();
    void playerDead();
    void resetStatePlaying();
    // Getter

  private:
    Device &device_;
    Renderer &renderer_;

    std::unique_ptr<InterfaceModel> model_;

    std::unique_ptr<DescriptorPool> textureDescriptorPool_{};
    std::vector<VkDescriptorSet> textureDescriptorSets_;
    std::unique_ptr<DescriptorSetLayout> textureDescriptorSetLayout_;

    std::vector<GameObject> menuInterface_;
    std::vector<std::vector<GameObject>> playerInterface_;
    std::vector<GameObject> gameInterface_;

    playerCoordinate playerCoordinate_;
    GameObject *playerPointer_;

    std::vector<std::pair<TextureIndex, glm::vec4>> playerInput_;
  };
}  // namespace ve