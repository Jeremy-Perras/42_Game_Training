#pragma once
#include "descriptors.hpp"
#include "interface_model.hpp"
#include "renderer.hpp"
namespace ve {
  class GameLoop {
  public:
    GameLoop(Device &device, Renderer &renderer, GameState &gameState,
             std::vector<GameObject> &menuInterface,
             std::vector<std::vector<GameObject>> &playerInterface,
             std::vector<GameObject> &gameInterface);
    GameLoop(const GameLoop &src) = delete;
    GameLoop &operator=(const GameLoop &rhs) = delete;
    ~GameLoop(){};

    void textureInit();
    void gameLoop();
    void playerDead();
    void resetStatePlaying();
    static bool isTextureWhite(TextureIndex texture);
    // Getter

    VkDescriptorSet getDescriptorSets(unsigned long currentFrame) {
      return textureDescriptorSets_[currentFrame];
    }

    std::vector<std::pair<TextureIndex, glm::vec4>> *getPlayerInput() { return &playerInput_; }

  private:
    void gameInit();
    void createDescriptor();
    void checkFunction();
    void checkArrow();
    void checkBrush();
    bool checkIsNotPlayerAndNotStarAndIsInside(GameObject const &obj);
    bool checkIsNotPlayerAndIsInside(GameObject const &obj);
    static bool checkIsTextureIsBrush(std::pair<TextureIndex, glm::vec4> playerInput);
    static bool checkIsTextureIsArrow(std::pair<TextureIndex, glm::vec4> playerInput);
    static bool checkColor(std::pair<TextureIndex, glm::vec4> playerInput, GameObject const &obj);
    void resetPlayerAngle();
    void checkIsTextureFunction(TextureIndex functionTexture, GameObject const &obj,
                                std::pair<TextureIndex, glm::vec4> playerInput);
    void addToPlayerInput(int index);

    GameState &gameState_;
    std::vector<GameObject> &gameInterface_;
    std::vector<GameObject> &menuInterface_;
    std::vector<std::vector<GameObject>> &playerInterface_;

    std::vector<std::shared_ptr<Texture>> texture_;

    Device &device_;
    Renderer &renderer_;

    std::unique_ptr<InterfaceModel> model_;

    std::unique_ptr<DescriptorPool> textureDescriptorPool_{};
    std::vector<VkDescriptorSet> textureDescriptorSets_;
    std::unique_ptr<DescriptorSetLayout> textureDescriptorSetLayout_;

    playerCoordinate playerCoordinate_;
    GameObject *playerPointer_;

    std::vector<std::pair<TextureIndex, glm::vec4>> playerInput_;
    std::string lvlPath_ = "lvl/lvl1.ber";
    int countStar_;
  };
}  // namespace ve