#pragma once
#include <string>
#include <vector>

#include "descriptors.hpp"
#include "game_object.hpp"
#include "interface_model.hpp"
#include "renderer.hpp"
namespace ve {
  class GameLoop {
  public:
    GameLoop(Device &device, Renderer &renderer, GameState &gameState,
             std::vector<GameObject> &menuInterface,
             std::vector<std::vector<GameObject>> &playerInterface,
             std::vector<GameObject> &gameInterface, std::vector<GameObject> &displayInterface_,
             std::vector<GameObject> &timeInterface, std::vector<GameObject> &menuStartInterface);
    GameLoop(const GameLoop &src) = delete;
    GameLoop &operator=(const GameLoop &rhs) = delete;
    ~GameLoop(){};

    void textureInit();
    void gameLoop();
    void playerDead();
    void resetStatePlaying();
    static bool isTextureWhite(TextureIndex texture);
    void updateGameLvl();
    bool checkIsPlayerDead();
    void updateDisplay();
    void deletePlayerInputFirstElement();
    // Getter

    VkDescriptorSet getDescriptorSets(unsigned long currentFrame) {
      return textureDescriptorSets_[currentFrame];
    }

    VkDescriptorSetLayout_T *getDescriptorSetsLayout() {
      return textureDescriptorSetLayout_->getDescriptorSetLayout();
    }

    std::vector<std::pair<TextureIndex, glm::vec4>> *getPlayerInput() { return &playerInput_; }
    std::vector<std::shared_ptr<Texture>> texture_;

    // Setter
    void setTexturePath(int lvl) {
      std::string s = "lvl/lvl";
      s.append(std::to_string(lvl));
      s.append(".ber");
      lvlPath_ = s;
    }

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
    std::vector<GameObject> &displayInterface_;
    std::vector<GameObject> &timeInterface_;
    std::vector<GameObject> &menuStartInterface_;

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
    bool function_ = false;

    std::vector<std::unique_ptr<Buffer>> uboBuffers;
  };
}  // namespace ve