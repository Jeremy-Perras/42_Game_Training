#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "game_object.hpp"
#include "texture.hpp"

namespace ve {

  class InterfaceModel {
  public:
    InterfaceModel(
        Device& device, Renderer& renderer_, std::string& lvlPath,
        std::vector<std::shared_ptr<Texture>>& texture, std::vector<std::shared_ptr<Texture>>& exit,
        std::vector<GameObject>& menuInterface,
        std::vector<std::vector<GameObject>>& playerInterface,
        std::vector<GameObject>& gameInterface, std::vector<GameObject>& displayInterface,
        std::vector<GameObject>& timeInterface, std::vector<GameObject>& menuStartInterface,
        std::vector<GameObject>& exitInterface, std::vector<GameObject>& chooseLevelInterface);

    InterfaceModel(const InterfaceModel& src) = delete;
    InterfaceModel& operator=(const InterfaceModel& rhs) = delete;
    InterfaceModel(InterfaceModel&& src) = delete;
    InterfaceModel& operator=(InterfaceModel&& rhs) = delete;
    ~InterfaceModel();

    void loadTexture();
    void createMenuInterface();
    void createPlayerInterface();
    void createTimeInterface();
    void createGameInterface(std::vector<std::string> map);
    void saveInitialState();
    void resetToInitialState();
    void uptateGameLvl(std::string& lvlPath);
    void createGameMap();
    void createDisplayInterface();
    void createStartInterface();
    void exitGame();
    static bool isInside(float x, float y, float i, float j) {
      return ((i * 0.05) - 1.0F > x && x < ((i + 1) * 0.05) - 1.0F) && (j * 0.05) - 1.0F > y
             && y < ((j + 1) * 0.05) - 1.0F;
    }

    // Getter
    playerCoordinate getStartCoordinate() { return playerStart_; }
    GameObject* getPlayerPointer() { return playerPointer_; }
    int getCountStarStart() const { return countStarStart_; }

  private:
    void chooseOrientationPlayer(char c, TextureRenderSystem::Builder builder);
    void setAlphaColor(GameObject& obj, int indexMap);
    Device& device_;
    Renderer& renderer_;

    std::vector<GameObject>& gameInterface_;
    std::vector<GameObject>& menuInterface_;
    std::vector<std::vector<GameObject>>& playerInterface_;
    std::vector<GameObject>& displayInterface_;
    std::vector<GameObject>& timeInterface_;
    std::vector<GameObject>& menuStartInterface_;
    std::vector<GameObject>& exitInterface_;
    std::vector<std::shared_ptr<Texture>>& texture_;
    std::vector<std::shared_ptr<Texture>>& exit_;
    std::vector<GameObject>& chooseLevelInterface_;

    playerCoordinate playerStart_;
    GameObject* playerPointer_;
    std::vector<std::pair<TextureIndex, glm::vec4>> initialState_;
    float xStart_;
    float yStart_;
    int countStarStart_ = 0;
    int lineSize_;
    int coloneSize_;
    std::vector<std::string> map_;
  };

}  // namespace ve