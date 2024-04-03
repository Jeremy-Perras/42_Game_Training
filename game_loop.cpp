#include "game_loop.hpp"

#include <algorithm>

#include "game_object.hpp"
#include "texture_render_system.hpp"
#include "utils.hpp"
namespace ve {
  GameLoop::GameLoop(Device &device, Renderer &renderer, GameState &gameState,
                     std::vector<GameObject> &menuInterface,
                     std::vector<std::vector<GameObject>> &playerInterface,
                     std::vector<GameObject> &gameInterface,
                     std::vector<GameObject> &displayInterface,
                     std::vector<GameObject> &timeInterface)
      : gameState_(gameState),
        gameInterface_(gameInterface),
        menuInterface_(menuInterface),
        playerInterface_(playerInterface),
        displayInterface_(displayInterface),
        timeInterface_(timeInterface),
        device_(device),
        renderer_(renderer) {
    createDescriptor();
    gameInit();
  }

  void GameLoop::gameInit() {
    model_ = std::make_unique<InterfaceModel>(device_, renderer_,
                                              textureDescriptorSetLayout_->getDescriptorSetLayout(),
                                              lvlPath_, texture_, menuInterface_, playerInterface_,
                                              gameInterface_, displayInterface_, timeInterface_);
    textureInit();
    model_->createMenuInterface();
    model_->createPlayerInterface();
    model_->createGameMap();
    model_->createDisplayInterface();
    model_->createTimeInterface();
    playerPointer_ = model_->getPlayerPointer();
    playerCoordinate_ = model_->getStartCoordinate();
    countStar_ = model_->getCountStarStart();
  }

  void GameLoop::createDescriptor() {
    textureDescriptorPool_ = DescriptorPool::Builder(device_)
                                 .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                              textureSize * SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .build();

    textureDescriptorSetLayout_ = DescriptorSetLayout::Builder(device_)
                                      .addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                  VK_SHADER_STAGE_FRAGMENT_BIT, textureSize)
                                      .build();

    textureDescriptorSets_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  }

  void GameLoop::textureInit() {
    model_->loadTexture();

    std::vector<VkDescriptorImageInfo> textureDescriptors(textureSize);
    for (int i = 0; i < textureSize; i++) {
      if (texture_[i]) {
        textureDescriptors[i] = texture_[i]->getImageInfo();
      }
    }

    for (int i = 0; i < static_cast<int>(textureDescriptorSets_.size()); i++) {
      DescriptorWriter(*textureDescriptorSetLayout_, *textureDescriptorPool_)
          .writeImage(3, textureDescriptors.data(), textureSize)
          .build(textureDescriptorSets_[i]);
    }
  }

  void GameLoop::gameLoop() {
    checkFunction();
    if (!function_) {
      checkArrow();
      checkBrush();
      for (const auto &obj : gameInterface_) {
        if (checkIsNotPlayerAndIsInside(obj)) {
          if (obj.textureRenderSystem->getIndexTexture() == TextureIndex::STAR) {
            obj.textureRenderSystem->setIndexTexture(TextureIndex::DISCARD);
            countStar_--;
            if (countStar_ == 0) {
              gameState_ = MENU;
            }
            break;
          }
        }
      }
      deletePlayerInputFirstElement();
    }
    updateDisplay();
    function_ = false;
  }

  void GameLoop::playerDead() {
    for (const auto &obj : gameInterface_) {
      if (&obj != playerPointer_) {
        if (checkIsPlayerDead()) {
          resetStatePlaying();
          break;
        }
      }
    }
  }

  void GameLoop::resetStatePlaying() {
    model_->resetToInitialState();
    playerCoordinate_ = model_->getStartCoordinate();
    playerPointer_->textureRenderSystem->resetPushCoordinate();
    countStar_ = model_->getCountStarStart();
    for (auto &obj : displayInterface_) {
      obj.textureRenderSystem->setIndexTexture(TextureIndex::WHITE);
      obj.textureRenderSystem->setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
    }
    gameState_ = GameState::PLAYING;
  }

  void GameLoop::checkFunction() {
    std::pair<TextureIndex, glm::vec4> playerInput = playerInput_[0];
    for (const auto &obj : gameInterface_) {
      checkIsTextureFunction(playerInput.first, obj, playerInput);
    }
  }

  void GameLoop::checkArrow() {
    std::pair<TextureIndex, glm::vec4> playerInput = playerInput_[0];
    if (checkIsTextureIsArrow(playerInput)) {
      for (const auto &obj : gameInterface_) {
        if (checkIsNotPlayerAndNotStarAndIsInside(obj)) {
          if (checkColor(playerInput, obj)) {
            if (playerInput.first == TextureIndex::ARROWUP) {
              playerPointer_->textureRenderSystem->setBuilderCoordinate(&playerCoordinate_);
              break;
            }
            if (playerInput.first == TextureIndex::ARROWLEFT) {
              playerCoordinate_.Angle = playerCoordinate_.Angle + 90.0F;

              break;
            }
            if (playerInput.first == TextureIndex::ARROWRIGHT) {
              playerCoordinate_.Angle = playerCoordinate_.Angle - 90.0F;
              break;
            }
          }
        }
      }
      playerPointer_->textureRenderSystem->setPlayerTextureOrientation(playerCoordinate_);
      resetPlayerAngle();
    }
  }

  void GameLoop::checkBrush() {
    std::pair<TextureIndex, glm::vec4> playerInput = playerInput_[0];
    if (checkIsTextureIsBrush(playerInput)) {
      for (const auto &obj : gameInterface_) {
        if (checkIsNotPlayerAndNotStarAndIsInside(obj)) {
          if (checkColor(playerInput, obj)) {
            if (playerInput.first == TextureIndex::BRUSHRED) {
              obj.textureRenderSystem->setIndexTexture(TextureIndex::RED);
              obj.textureRenderSystem->setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
              break;
            }
            if (playerInput.first == TextureIndex::BRUSHGREEN) {
              obj.textureRenderSystem->setIndexTexture(TextureIndex::GREEN);
              obj.textureRenderSystem->setColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
              break;
            }
            if (playerInput.first == TextureIndex::BRUSHBLUE) {
              obj.textureRenderSystem->setIndexTexture(TextureIndex::BLUE);
              obj.textureRenderSystem->setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
              break;
            }
          }
        }
      }
    }
  }

  bool GameLoop::checkIsNotPlayerAndNotStarAndIsInside(GameObject const &obj) {
    if (&obj != playerPointer_
        && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)
        && obj.textureRenderSystem->getIndexTexture() != STAR
        && obj.textureRenderSystem->getIndexTexture() != DISCARD) {
      return true;
    }
    return false;
  }

  bool GameLoop::checkIsNotPlayerAndIsInside(GameObject const &obj) {
    if (&obj != playerPointer_
        && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
      return true;
    }
    return false;
  }

  bool GameLoop::checkIsTextureIsBrush(std::pair<TextureIndex, glm::vec4> playerInput) {
    if (playerInput.first == TextureIndex::BRUSHRED || playerInput.first == TextureIndex::BRUSHBLUE
        || playerInput.first == TextureIndex::BRUSHGREEN) {
      return true;
    }
    return false;
  }

  bool GameLoop::checkIsTextureIsArrow(std::pair<TextureIndex, glm::vec4> playerInput) {
    if (playerInput.first == TextureIndex::ARROWUP || playerInput.first == TextureIndex::ARROWRIGHT
        || playerInput.first == TextureIndex::ARROWLEFT) {
      return true;
    }
    return false;
  }

  bool GameLoop::checkColor(std::pair<TextureIndex, glm::vec4> playerInput, GameObject const &obj) {
    if (obj.textureRenderSystem->getColor() == playerInput.second
        || (playerInput.second == glm::vec4(1.0, 1.0, 1.0, 1.0)
            && playerInput.first != TextureIndex::WHITE)) {
      return true;
    }
    return false;
  }

  void GameLoop::resetPlayerAngle() {
    if (playerCoordinate_.Angle == 360.0F || playerCoordinate_.Angle == -360.0F) {
      playerCoordinate_.Angle = 0.0F;
    }
  }
  void GameLoop::checkIsTextureFunction(TextureIndex functionTexture, GameObject const &obj,
                                        std::pair<TextureIndex, glm::vec4> playerInput) {
    {
      if (checkIsNotPlayerAndNotStarAndIsInside(obj)) {
        if (checkColor(playerInput, obj)) {
          if (playerInput.first == TextureIndex::F0) {
            deletePlayerInputFirstElement();
            addToPlayerInput(0);
            function_ = true;
          } else if (functionTexture == TextureIndex::F1) {
            deletePlayerInputFirstElement();
            addToPlayerInput(1);
            function_ = true;
          } else if (functionTexture == TextureIndex::F2) {
            deletePlayerInputFirstElement();
            addToPlayerInput(2);
            function_ = true;
          }
        }
      }
    }
  }

  void GameLoop::deletePlayerInputFirstElement() {
    if (!playerInput_.empty()) {
      playerInput_.erase(playerInput_.begin());
    }
  }

  void GameLoop::addToPlayerInput(int index) {
    if (index == 0) {
      for (int i = 0; i < static_cast<int>(playerInterface_[0].size()); i++) {
        if (!isTextureWhite(playerInterface_[0][playerInterface_[0].size() - i - 1]
                                .textureRenderSystem->getIndexTexture())) {
          playerInput_.emplace(
              playerInput_.begin(),
              std::make_pair(playerInterface_[0][playerInterface_[0].size() - i - 1]
                                 .textureRenderSystem->getIndexTexture(),
                             playerInterface_[0][playerInterface_[0].size() - i - 1]
                                 .textureRenderSystem->getColor()));
        }
      }
    } else if (index == 1) {
      for (int i = 0; i < static_cast<int>(playerInterface_[1].size()); i++) {
        if (!isTextureWhite(playerInterface_[1][playerInterface_[1].size() - i - 1]
                                .textureRenderSystem->getIndexTexture())) {
          playerInput_.emplace(
              playerInput_.begin(),
              std::make_pair(playerInterface_[1][playerInterface_[1].size() - i - 1]
                                 .textureRenderSystem->getIndexTexture(),
                             playerInterface_[1][playerInterface_[1].size() - i - 1]
                                 .textureRenderSystem->getColor()));
        }
      }
    } else if (index == 2) {
      for (int i = 0; i < static_cast<int>(playerInterface_[2].size()); i++) {
        if (!isTextureWhite(playerInterface_[2][playerInterface_[2].size() - i - 1]
                                .textureRenderSystem->getIndexTexture())) {
          playerInput_.emplace(
              playerInput_.begin(),
              std::make_pair(playerInterface_[2][playerInterface_[2].size() - i - 1]
                                 .textureRenderSystem->getIndexTexture(),
                             playerInterface_[2][playerInterface_[2].size() - i - 1]
                                 .textureRenderSystem->getColor()));
        }
      }
    }
  }

  bool GameLoop::isTextureWhite(TextureIndex texture) {
    if (texture == TextureIndex::WHITE) {
      return true;
    }
    return false;
  }

  void GameLoop::updateGameLvl() {
    playerInput_.clear();
    model_->uptateGameLvl(lvlPath_);
    model_->createMenuInterface();
    model_->createPlayerInterface();
    model_->createGameMap();
    for (auto &obj : displayInterface_) {
      obj.textureRenderSystem->setIndexTexture(TextureIndex::WHITE);
      obj.textureRenderSystem->setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
    }
    playerPointer_ = model_->getPlayerPointer();
    playerCoordinate_ = model_->getStartCoordinate();
    countStar_ = model_->getCountStarStart();
    gameState_ = PLAYING;
  }

  bool GameLoop::checkIsPlayerDead() {
    if (std::any_of(gameInterface_.begin(), gameInterface_.end(),
                    [this](GameObject &obj) { return (checkIsNotPlayerAndIsInside(obj)); })) {
      return false;
    }

    return true;
  }

  void GameLoop::updateDisplay() {
    int i = 0;

    for (auto &obj : displayInterface_) {
      obj.textureRenderSystem->setIndexTexture(TextureIndex::WHITE);
      obj.textureRenderSystem->setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
      if (i < static_cast<int>(playerInput_.size())) {
        obj.textureRenderSystem->setIndexTexture(playerInput_[i].first);
        obj.textureRenderSystem->setColor(playerInput_[i].second);
        i++;
      }
    }
  }

}  // namespace ve