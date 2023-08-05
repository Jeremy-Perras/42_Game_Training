#include "interface_model.hpp"

#include <iostream>
#include <utility>

#include "game_object.hpp"
#include "parsing.hpp"

namespace ve {

  InterfaceModel::InterfaceModel(Device &device, VkRenderPass renderPass,
                                 VkDescriptorSetLayout descriptorLayout, std::string &lvlPath,
                                 std::vector<std::shared_ptr<Texture>> &texture,
                                 std::vector<GameObject> &menuInterface,
                                 std::vector<std::vector<GameObject>> &playerInterface,
                                 std::vector<GameObject> &gameInterface)
      : device_(device),
        renderPass_(renderPass),
        gameInterface_(gameInterface),
        menuInterface_(menuInterface),
        playerInterface_(playerInterface),
        texture_(texture),
        descriptorLayout_(descriptorLayout) {
    Parsing parsing(lvlPath);
    xStart_ = parsing.getXStart();
    yStart_ = parsing.getYStart();
    lineSize_ = parsing.getLineSize();
    coloneSize_ = parsing.getColoneSize();
    map_ = parsing.getMap();
  }

  InterfaceModel::~InterfaceModel() {}

  std::vector<RenderSystem::Vertex> InterfaceModel::createSquareModel() {
    std::vector<RenderSystem::Vertex> vertices = {
        {{-1.0F, 0.70F}, {1.0F, 1.0F, 1.0F}}, {{1.0F, 0.70F}, {1.0F, 1.0F, 1.0F}},
        {{1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}},   {{-1.0F, 0.70F}, {1.0F, 1.0F, 1.0F}},
        {{-1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}},  {{1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}},  //
    };
    return vertices;
  }

  void InterfaceModel::loadTexture() {
    auto arrowRight = std::make_unique<Texture>(device_, "texture/RightArrow.png");
    auto f0 = std::make_unique<Texture>(device_, "texture/F0.png");
    auto red = std::make_unique<Texture>(device_, "texture/Red.png");
    auto arrowUp = std::make_unique<Texture>(device_, "texture/UpArrow.png");
    auto f1 = std::make_unique<Texture>(device_, "texture/F1.png");
    auto green = std::make_unique<Texture>(device_, "texture/Green.png");
    auto arrowLeft = std::make_unique<Texture>(device_, "texture/LeftArrow.png");
    auto f2 = std::make_unique<Texture>(device_, "texture/F2.png");
    auto blue = std::make_unique<Texture>(device_, "texture/Blue.png");
    auto rocket = std::make_unique<Texture>(device_, "texture/Rocket.png");
    auto background = std::make_unique<Texture>(device_, "texture/Dream.jpg");
    auto start = std::make_unique<Texture>(device_, "texture/Star.png");

    texture_.push_back(std::move(arrowLeft));
    texture_.push_back(std::move(arrowUp));
    texture_.push_back(std::move(arrowRight));
    texture_.push_back(std::move(f0));
    texture_.push_back(std::move(f1));
    texture_.push_back(std::move(f2));
    texture_.push_back(std::move(red));
    texture_.push_back(std::move(green));
    texture_.push_back(std::move(blue));
    texture_.push_back(std::move(rocket));
    texture_.push_back(std::move(background));
    texture_.push_back(std::move(start));
  }

  void InterfaceModel::createMenuInterface() {
    TextureRenderSystem::Builder builder;
    float xStart = -0.95;
    float yStart = 0.80F;
    int squareSize = 3;
    // create menu
    for (int j = 0; j < squareSize; j++) {
      for (int i = 0; i < squareSize; i++) {
        builder = {{{{xStart + static_cast<float>(i) * WIDTHVERTEX,
                      yStart + static_cast<float>(j) * HEIGHTVERTEX},

                     {0.0F, 0.0F}},
                    {{xStart + WIDTHVERTEX + static_cast<float>(i) * WIDTHVERTEX,
                      yStart + static_cast<float>(j) * HEIGHTVERTEX},

                     {1.0F, 0.0F}},
                    {{xStart + WIDTHVERTEX + static_cast<float>(i) * WIDTHVERTEX,
                      yStart + HEIGHTVERTEX + static_cast<float>(j) * HEIGHTVERTEX},

                     {1.0F, 1.0F}},
                    {{xStart + static_cast<float>(i) * WIDTHVERTEX,
                      yStart + HEIGHTVERTEX + static_cast<float>(j) * HEIGHTVERTEX},

                     {0.0F, 1.0F}}},
                   {0, 1, 2, 2, 3, 0}};
        auto object = GameObject::createGameObject();
        object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderPass_, descriptorLayout_, builder,
            static_cast<TextureIndex>(i + j * squareSize));
        menuInterface_.push_back(std::move(object));
      }
    }
    // create color square
    for (int j = 0; j < squareSize; j++) {
      builder
          = {{{{xStart + 0.01F + 3 * WIDTHVERTEX, yStart + static_cast<float>(j) * HEIGHTVERTEX},

               {1.0F, 0.0F}},
              {{xStart + WIDTHVERTEX + 0.01F + 3 * WIDTHVERTEX,
                yStart + static_cast<float>(j) * HEIGHTVERTEX},

               {0.0F, 0.0F}},
              {{xStart + WIDTHVERTEX + 0.01F + 3 * WIDTHVERTEX,
                yStart + HEIGHTVERTEX + static_cast<float>(j) * HEIGHTVERTEX},

               {0.0F, 1.0F}},
              {{xStart + 0.01F + 3 * WIDTHVERTEX,
                yStart + HEIGHTVERTEX + static_cast<float>(j) * HEIGHTVERTEX},

               {1.0F, 1.0F}}},
             {0, 1, 2, 2, 3, 0}};
      auto object = GameObject::createGameObject();
      object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
          device_, renderPass_, descriptorLayout_, builder,
          static_cast<TextureIndex>(j + squareSize * squareSize));
      menuInterface_.push_back(std::move(object));
    }
    // create Button play stop and pause
    for (int i = 0; i < squareSize; i++) {
      builder = {{{{xStart + static_cast<float>(i) * WIDTHVERTEX, yStart - 0.06},

                   {1.0F, 0.0F}},
                  {
                      {xStart + static_cast<float>(i) * WIDTHVERTEX + WIDTHVERTEX, yStart - 0.06},

                      {0.0F, 0.0F},
                  },
                  {{xStart + static_cast<float>(i) * WIDTHVERTEX + WIDTHVERTEX, yStart - 0.01},

                   {0.0F, 1.0F}},
                  {{xStart + static_cast<float>(i) * WIDTHVERTEX, yStart - 0.01},

                   {1.0F, 1.0F}}},
                 {0, 1, 2, 2, 3, 0}};
      auto object = GameObject::createGameObject();
      object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
          device_, renderPass_, descriptorLayout_, builder,
          static_cast<TextureIndex>(i + TextureIndex::PLAY));
      menuInterface_.push_back(std::move(object));
    }
  }

  void InterfaceModel::createPlayerInterface() {
    std::vector<TextureRenderSystem::Builder> builder;
    float xStart = -0.25;
    float yStart = 0.8F;
    int sizeF[3] = {map_[2][6] - '0' + 1, map_[2][14] - '0' + 1, map_[2][22] - '0' + 1};
    playerInterface_.resize(3);
    int inc = 0;

    for (int j = 0; j < 3; j++) {
      for (int i = 0; i < sizeF[j]; i++) {
        builder.push_back(
            {{{{xStart + static_cast<float>(i) * 0.05F, yStart + static_cast<float>(j) * 0.05},

               {0.0F, 0.0F}},
              {{xStart + 0.05F + static_cast<float>(i) * 0.05F,
                yStart + static_cast<float>(j) * 0.05},

               {1.0F, 0.0F}},
              {{xStart + 0.05F + static_cast<float>(i) * 0.05F,
                yStart + 0.05F + static_cast<float>(j) * 0.05},

               {1.0F, 1.0F}},
              {{xStart + static_cast<float>(i) * 0.05F,
                yStart + 0.05F + static_cast<float>(j) * 0.05},

               {0.0F, 1.0F}}},
             {0, 1, 2, 2, 3, 0}});
        auto object = GameObject::createGameObject();
        object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderPass_, descriptorLayout_, builder[inc++], TextureIndex::WHITE);
        playerInterface_[j].push_back(std::move(object));
      }
    }

    auto SquareF0 = GameObject::createGameObject();
    SquareF0.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[0], TextureIndex::F0);
    menuInterface_.push_back(std::move(SquareF0));
    playerInterface_[0].erase(playerInterface_[0].begin());

    auto SquareF1 = GameObject::createGameObject();
    SquareF1.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[sizeF[0]], TextureIndex::F1);
    menuInterface_.push_back(std::move(SquareF1));
    playerInterface_[1].erase(playerInterface_[1].begin());
    auto SquareF2 = GameObject::createGameObject();
    SquareF2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[sizeF[0] + sizeF[1]], TextureIndex::F2);
    menuInterface_.push_back(std::move(SquareF2));
    playerInterface_[2].erase(playerInterface_[2].begin());
    RenderSystem::Builder renderBuilder;
    renderBuilder.vertices = InterfaceModel::createSquareModel();
    auto whiteSquare = GameObject::createGameObject();
    whiteSquare.renderSystem
        = std::make_unique<RenderSystem>(device_, renderPass_, descriptorLayout_, renderBuilder);
    menuInterface_.push_back(std::move(whiteSquare));
  }

  void InterfaceModel::createGameMap() {
    TextureRenderSystem::Builder builder;
    int lineMapSize = static_cast<int>((2 / HEIGHTVERTEX) - (0.2) / HEIGHTVERTEX);
    int coloneMapSize = static_cast<int>(2 / WIDTHVERTEX);
    int lineMap = 0;
    int coloneMap = 0;
    float xStart = xStart_;
    float yStart = yStart_;

    for (int j = 0; j < lineMapSize; j++) {
      for (int i = 0; i < coloneMapSize; i++) {
        builder
            = {{{{-1 + static_cast<float>(i) * 0.05F, -1 + static_cast<float>(j) * 0.05},

                 {0.0F, 0.0F}},
                {{-1 + 0.05F + static_cast<float>(i) * 0.05F, -1 + static_cast<float>(j) * 0.05},

                 {1.0F, 0.0F}},
                {{-1 + 0.05F + static_cast<float>(i) * 0.05F,
                  0.05F + -1 + static_cast<float>(j) * 0.05},

                 {1.0F, 1.0F}},
                {{-1 + static_cast<float>(i) * 0.05F, -1 + static_cast<float>(j) * 0.05 + 0.05},

                 {0.0F, 1.0F}}},
               {0, 1, 2, 2, 3, 0}};

        auto object = GameObject::createGameObject();
        object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderPass_, descriptorLayout_, builder, TextureIndex::LOST);

        if (isInside(xStart, yStart, static_cast<float>(i), static_cast<float>(j))
            && coloneMap != coloneSize_) {
          xStart = xStart + 0.05F;

          switch (map_[1][lineMap + coloneMap * static_cast<int>(lineSize_)]) {
            case 'V': {
              auto object2 = GameObject::createGameObject();
              object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                  device_, renderPass_, descriptorLayout_, builder, TextureIndex::STAR);
              gameInterface_.push_back(std::move(object2));
              countStar_++;
              break;
            }
            case 'N': {
              chooseOrientationPlayer('N', builder);
              break;
            }
            case 'E': {
              chooseOrientationPlayer('E', builder);
              break;
            }
            case 'S': {
              chooseOrientationPlayer('S', builder);
              break;
            }
            case 'W': {
              chooseOrientationPlayer('W', builder);
              break;
            }
            default:
              break;
          }

          switch (map_[0][lineMap + coloneMap * static_cast<int>(lineSize_)]) {
            case 'R':
              object.textureRenderSystem->setIndexTexture(TextureIndex::RED);
              object.textureRenderSystem->setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
              break;
            case 'G':
              object.textureRenderSystem->setIndexTexture(TextureIndex::GREEN);
              object.textureRenderSystem->setColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
              break;
            case 'B':
              object.textureRenderSystem->setIndexTexture(TextureIndex::BLUE);
              object.textureRenderSystem->setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
              break;
            default:
              break;
          }

          lineMap++;
          if (lineMap == lineSize_) {
            xStart = xStart_;
            yStart = yStart + 0.05F;
            lineMap = 0;
            coloneMap++;
          }
        }

        gameInterface_.push_back(std::move(object));
      }
    }
    playerPointer_ = gameInterface_.data();
    saveInitialState();
  }

  void InterfaceModel::saveInitialState() {
    for (const auto &obj : gameInterface_) {
      initialState_.push_back(std::make_pair(obj.textureRenderSystem->getIndexTexture(),
                                             obj.textureRenderSystem->getColor()));
    }
  }

  void InterfaceModel::resetToInitialState() {
    for (int i = 0; i < static_cast<int>(gameInterface_.size()); i++) {
      gameInterface_[i].textureRenderSystem->setIndexTexture(initialState_[i].first);
      gameInterface_[i].textureRenderSystem->setColor(initialState_[i].second);
    }
  }

  void InterfaceModel::chooseOrientationPlayer(char c, TextureRenderSystem::Builder builder) {
    auto object2 = GameObject::createGameObject();
    playerStart_.x = (builder.vertices[0].pos.x + builder.vertices[1].pos.x) / 2;
    playerStart_.y = (builder.vertices[0].pos.y + builder.vertices[2].pos.y) / 2;

    switch (c) {
      case 'N': {
        playerStart_.Angle = 90.0F;
        object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderPass_, descriptorLayout_, builder, TextureIndex::PLAYER);
        gameInterface_.emplace(gameInterface_.begin(), std::move(object2));
        break;
      }
      case 'E': {
        playerStart_.Angle = 0.0F;
        object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderPass_, descriptorLayout_, builder, TextureIndex::PLAYER);
        gameInterface_.emplace(gameInterface_.begin(), std::move(object2));
        break;
      }
      case 'W': {
        playerStart_.Angle = 180.0F;
        object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderPass_, descriptorLayout_, builder, TextureIndex::PLAYER);
        gameInterface_.emplace(gameInterface_.begin(), std::move(object2));
        break;
      }
      case 'S': {
        playerStart_.Angle = 270.0F;
        object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderPass_, descriptorLayout_, builder, TextureIndex::PLAYER);
        gameInterface_.emplace(gameInterface_.begin(), std::move(object2));
        break;
      }
    }
  }

}  // namespace ve