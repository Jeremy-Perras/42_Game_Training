#include "game_loop.hpp"
namespace ve {
  GameLoop::GameLoop(Device &device, Renderer &renderer) : device_(device), renderer_(renderer) {
    createDescriptor();
    gameInit();
  }

  void GameLoop::gameInit() {
    std::string lvlPath = "lvl/lvl1.ber";
    model_ = std::make_unique<InterfaceModel>(device_, renderer_.getSwapChainRenderPass(),
                                              textureDescriptorSetLayout_->getDescriptorSetLayout(),
                                              lvlPath);
    model_->createMenuInterface(&menuInterface_);
    model_->createPlayerInterface(&playerInterface_, &menuInterface_);
    model_->createGameMap(&gameInterface_);
    playerPointer_ = model_->getPlayerPointer();
    playerCoordinate_ = model_->getStartCoordinate();
    textureInit();
  }

  void GameLoop::createDescriptor() {
    textureDescriptorPool_ = DescriptorPool::Builder(device_)
                                 .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                              textureSize * SwapChain::MAX_FRAMES_IN_FLIGHT)
                                 .build();

    textureDescriptorSetLayout_ = DescriptorSetLayout::Builder(device_)
                                      .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                  VK_SHADER_STAGE_FRAGMENT_BIT, textureSize)
                                      .build();

    textureDescriptorSets_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
  }

  void GameLoop::textureInit() {
    std::vector<std::shared_ptr<Texture>> texture;
    model_->loadTexture(&texture);

    std::vector<VkDescriptorImageInfo> textureDescriptors(textureSize);
    for (int i = 0; i < textureSize; i++) {
      if (texture[i]) {
        textureDescriptors[i] = texture[i]->getImageInfo();
      }
    }

    for (int i = 0; i < static_cast<int>(textureDescriptorSets_.size()); i++) {
      DescriptorWriter(*textureDescriptorSetLayout_, *textureDescriptorPool_)
          .writeImage(0, textureDescriptors.data(), textureSize)
          .build(textureDescriptorSets_[i]);
    }
  }

  void GameLoop::gameLoop() {
    if (playerInput_[0].first == TextureIndex::F0) {
      for (int i = 0; i < static_cast<int>(playerInterface_[0].size()); i++) {
        playerInput_.push_back(
            std::make_pair(playerInterface_[0][i].textureRenderSystem->getIndexTexture(),
                           playerInterface_[0][i].textureRenderSystem->getColor()));
      }
    } else if (playerInput_[0].first == TextureIndex::F1) {
      for (int i = 0; i < static_cast<int>(playerInterface_[1].size()); i++) {
        playerInput_.push_back(
            std::make_pair(playerInterface_[1][i].textureRenderSystem->getIndexTexture(),
                           playerInterface_[1][i].textureRenderSystem->getColor()));
      }
    } else if (playerInput_[0].first == TextureIndex::F2) {
      for (int i = 0; i < static_cast<int>(playerInterface_[2].size()); i++) {
        playerInput_.push_back(
            std::make_pair(playerInterface_[2][i].textureRenderSystem->getIndexTexture(),
                           playerInterface_[2][i].textureRenderSystem->getColor()));
      }
    }

    if (playerInput_[0].first == TextureIndex::ARROWUP
        || playerInput_[0].first == TextureIndex::ARROWRIGHT
        || playerInput_[0].first == TextureIndex::ARROWLEFT) {
      for (const auto &obj : gameInterface_) {
        if (&obj != playerPointer_
            && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
          if (obj.textureRenderSystem->getColor() == playerInput_[0].second
              || (playerInput_[0].second == glm::vec4(1.0, 1.0, 1.0, 1.0)
                  && playerInput_[0].first != TextureIndex::WHITE)) {
            if (playerInput_[0].first == TextureIndex::ARROWUP) {
              playerPointer_->textureRenderSystem->setBuilderCoordinate(&playerCoordinate_);
              break;
            }
            if (playerInput_[0].first == TextureIndex::ARROWLEFT) {
              playerCoordinate_.Angle = playerCoordinate_.Angle + 90.0F;
            }
            if (playerInput_[0].first == TextureIndex::ARROWRIGHT) {
              playerCoordinate_.Angle = playerCoordinate_.Angle - 90.0F;
            }
            if (playerCoordinate_.Angle == 360.0F || playerCoordinate_.Angle == -360.0F) {
              playerCoordinate_.Angle = 0.0F;
            }
          }
        }
      }
    }

    if (playerInput_[0].first == TextureIndex::BRUSHRED
        || playerInput_[0].first == TextureIndex::BRUSHBLUE
        || playerInput_[0].first == TextureIndex::BRUSHGREEN) {
      for (const auto &obj : gameInterface_) {
        if (&obj != playerPointer_
            && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
          if (obj.textureRenderSystem->getColor() == playerInput_[0].second
              || playerInput_[0].second == glm::vec4(1.0, 1.0, 1.0, 1.0)) {
            if (playerInput_[0].first == TextureIndex::BRUSHRED) {
              obj.textureRenderSystem->setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
            }
            if (playerInput_[0].first == TextureIndex::BRUSHGREEN) {
              obj.textureRenderSystem->setColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
            }
            if (playerInput_[0].first == TextureIndex::BRUSHBLUE) {
              obj.textureRenderSystem->setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
            }
          }
        }
      }
    }

    for (const auto &obj : gameInterface_) {
      if (&obj != playerPointer_
          && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
        if (obj.textureRenderSystem->getIndexTexture() == TextureIndex::STAR) {
          obj.textureRenderSystem->setIndexTexture(TextureIndex::DISCARD);
          break;
        }
      }
    }

    if (!playerInput_.empty()) {
      playerInput_.erase(playerInput_.begin());
    }
  }

  void GameLoop::playerDead() {
    for (const auto &obj : gameInterface_) {
      if (&obj != playerPointer_
          && obj.textureRenderSystem->isInside(playerCoordinate_.x, playerCoordinate_.y)) {
        if (obj.textureRenderSystem->getIndexTexture() == TextureIndex::LOST) {
          resetStatePlaying();
          break;
        }
      }
    }
  }

  void GameLoop::resetStatePlaying() {
    model_->resetToInitialState(&gameInterface_);
    playerCoordinate_ = model_->getStartCoordinate();
    playerPointer_->textureRenderSystem->resetPushCoordinate();
    // gameState_ = GameState::PLAYING;
  }

}  // namespace ve