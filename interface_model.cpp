#include "interface_model.hpp"

#include <iostream>
#include <utility>

#include "parsing.hpp"

namespace ve {

  InterfaceModel::InterfaceModel(Device &device, VkRenderPass renderPass,
                                 VkDescriptorSetLayout descriptorLayout, std::string &lvlPath)
      : device_(device), renderPass_(renderPass), descriptorLayout_(descriptorLayout) {
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

  void InterfaceModel::loadTexture(std::vector<std::shared_ptr<Texture>> *texture) {
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

    texture->push_back(std::move(arrowLeft));
    texture->push_back(std::move(arrowUp));
    texture->push_back(std::move(arrowRight));
    texture->push_back(std::move(f0));
    texture->push_back(std::move(f1));
    texture->push_back(std::move(f2));
    texture->push_back(std::move(red));
    texture->push_back(std::move(green));
    texture->push_back(std::move(blue));
    texture->push_back(std::move(rocket));
    texture->push_back(std::move(background));
    texture->push_back(std::move(start));
  }

  void InterfaceModel::createMenuInterface(std::vector<GameObject> *menuInterface) {
    std::vector<TextureRenderSystem::Builder> builder;
    float xStart = -0.95;
    float yStart = 0.80F;
    int squareSize = 3;
    // create menu
    for (int j = 0; j < squareSize; j++) {
      for (int i = 0; i < squareSize; i++) {
        builder.push_back({{{{xStart + static_cast<float>(i) * WIDTHVERTEX,
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
                           {0, 1, 2, 2, 3, 0}});
        auto object = GameObject::createGameObject();
        object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderPass_, descriptorLayout_, builder[i + j * squareSize],
            static_cast<TextureIndex>(i + j * squareSize));
        menuInterface->push_back(std::move(object));
      }
    }
    // create color square
    for (int j = 0; j < squareSize; j++) {
      builder.push_back(
          {{{{xStart + 0.01F + 3 * WIDTHVERTEX, yStart + static_cast<float>(j) * HEIGHTVERTEX},

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
           {0, 1, 2, 2, 3, 0}});
      auto object = GameObject::createGameObject();
      object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
          device_, renderPass_, descriptorLayout_, builder[j + TextureIndex::RED],
          static_cast<TextureIndex>(j + squareSize * squareSize));
      menuInterface->push_back(std::move(object));
    }
    // create Button play stop and pause
    for (int i = 0; i < squareSize; i++) {
      builder.push_back(
          {{{{xStart + static_cast<float>(i) * WIDTHVERTEX, yStart - 0.06},

             {1.0F, 0.0F}},
            {
                {xStart + static_cast<float>(i) * WIDTHVERTEX + WIDTHVERTEX, yStart - 0.06},

                {0.0F, 0.0F},
            },
            {{xStart + static_cast<float>(i) * WIDTHVERTEX + WIDTHVERTEX, yStart - 0.01},

             {0.0F, 1.0F}},
            {{xStart + static_cast<float>(i) * WIDTHVERTEX, yStart - 0.01},

             {1.0F, 1.0F}}},
           {0, 1, 2, 2, 3, 0}});
      auto object = GameObject::createGameObject();
      object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
          device_, renderPass_, descriptorLayout_,
          builder[i + squareSize * squareSize + squareSize],
          static_cast<TextureIndex>(i + TextureIndex::PLAY));
      menuInterface->push_back(std::move(object));
    }
  }

  void InterfaceModel::createPlayerInterface(std::vector<std::vector<GameObject>> *playerInterface,
                                             std::vector<GameObject> *menuInterface,
                                             interfaceSize size) {
    std::vector<TextureRenderSystem::Builder> builder;
    float xStart = -0.25;
    float yStart = 0.8F;
    int sizeF[3] = {size.f0 + 1, size.f1 + 1, size.f2 + 1};
    playerInterface->resize(3);
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
        playerInterface[0][j].push_back(std::move(object));
      }
    }

    auto SquareF0 = GameObject::createGameObject();
    SquareF0.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[0], TextureIndex::F0);
    menuInterface->push_back(std::move(SquareF0));
    playerInterface[0][0].erase(playerInterface[0][0].begin());

    auto SquareF1 = GameObject::createGameObject();
    SquareF1.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[sizeF[0]], TextureIndex::F1);
    menuInterface->push_back(std::move(SquareF1));
    playerInterface[0][1].erase(playerInterface[0][1].begin());
    auto SquareF2 = GameObject::createGameObject();
    SquareF2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[sizeF[0] + sizeF[1]], TextureIndex::F2);
    menuInterface->push_back(std::move(SquareF2));
    playerInterface[0][2].erase(playerInterface[0][2].begin());
    RenderSystem::Builder renderBuilder;
    renderBuilder.vertices = InterfaceModel::createSquareModel();
    auto whiteSquare = GameObject::createGameObject();
    whiteSquare.renderSystem
        = std::make_unique<RenderSystem>(device_, renderPass_, descriptorLayout_, renderBuilder);
    menuInterface->push_back(std::move(whiteSquare));
  }

  void InterfaceModel::createGameMap(std::vector<GameObject> *gameInterface) {
    TextureRenderSystem::Builder builder;
    int lineMapSize = static_cast<int>((2 / HEIGHTVERTEX) - (0.2) / HEIGHTVERTEX);
    int coloneMapSize = static_cast<int>(2 / WIDTHVERTEX);
    int lineMap = 0;
    int coloneMap = 0;
    float xStart = xStart_;
    float yStart = yStart_;
    gameInterface->size();
    unsigned long indexPlayer;

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
            case 'S': {
              auto object2 = GameObject::createGameObject();
              object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                  device_, renderPass_, descriptorLayout_, builder, TextureIndex::STAR);
              gameInterface->push_back(std::move(object2));
              countStar_++;
              break;
            }
            case 'N': {
              auto object2 = GameObject::createGameObject();
              playerStart_.x = (builder.vertices[0].pos.x + builder.vertices[1].pos.x) / 2;
              playerStart_.y = (builder.vertices[0].pos.y + builder.vertices[2].pos.y) / 2;
              playerStart_.Angle = 90.0F;

              object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                  device_, renderPass_, descriptorLayout_, builder, TextureIndex::PLAYER);
              gameInterface->emplace(gameInterface->begin(), std::move(object2));
              // gameInterface->push_back(std::move(object2));

              indexPlayer = 0;

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

        gameInterface->push_back(std::move(object));
      }
    }
    playerPointer_ = &(*gameInterface)[indexPlayer];
    saveInitialState(*gameInterface);
  }

  // void InterfaceModel::createGameInterface(std::vector<GameObject> *gameInterface) {
  //   unsigned long line = map[0].substr(0, map[0].find('\n')).size();

  //   int colone = 0;
  //   int inc = 0;
  //   std::string::size_type n = 0;

  //   for (auto c : map[0]) {
  //     if (c == '\n') {
  //       colone++;
  //     }
  //   }

  //   int lineStart = static_cast<int>(line) / 2;
  //   int coloneStart = static_cast<int>(colone) / 2;

  //   float xStart = -0.05F * static_cast<float>(lineStart);
  //   float yStart = -0.05F * static_cast<float>(coloneStart);
  //   std::vector<TextureRenderSystem::Builder> builder;

  //   while ((n = map[0].find('\n')) != std::string::npos) {
  //     map[0].erase(n, 1);
  //   }
  //   n = 0;
  //   while ((n = map[1].find('\n')) != std::string::npos) {
  //     map[1].erase(n, 1);
  //   }

  //   for (int j = 0; j < colone; j++) {
  //     for (int i = 0; i < static_cast<int>(line); i++) {
  //       builder.push_back(
  //           {{{{xStart + static_cast<float>(i) * 0.05F, yStart + static_cast<float>(j) * 0.05},

  //              {0.0F, 0.0F}},
  //             {{xStart + 0.05F + static_cast<float>(i) * 0.05F,
  //               yStart + static_cast<float>(j) * 0.05},

  //              {1.0F, 0.0F}},
  //             {{xStart + 0.05F + static_cast<float>(i) * 0.05F,
  //               yStart + 0.05F + static_cast<float>(j) * 0.05},

  //              {1.0F, 1.0F}},
  //             {{xStart + static_cast<float>(i) * 0.05F,
  //               yStart + 0.05F + static_cast<float>(j) * 0.05},

  //              {0.0F, 1.0F}}},
  //            {0, 1, 2, 2, 3, 0}});

  //       if (map[1][i + j * static_cast<int>(line)] != '1') {
  //         auto object2 = GameObject::createGameObject();
  //         switch (map[1][i + j * static_cast<int>(line)]) {
  //           case 'S':
  //             countStar_++;
  //             object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
  //                 device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::STAR);
  //             gameInterface->push_back(std::move(object2));
  //             break;
  //           case 'P':
  //             playerStart_.x
  //                 = (builder[inc].vertices[0].pos.x + builder[inc].vertices[1].pos.x) / 2;
  //             playerStart_.y
  //                 = (builder[inc].vertices[0].pos.y + builder[inc].vertices[2].pos.y) / 2;

  //             playerStart_.Angle = 90.0F;
  //             object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
  //                 device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::PLAYER);
  //             gameInterface->emplace(gameInterface->begin(), std::move(object2));
  //             playerPointer_ = gameInterface->data();

  //             break;
  //           default:
  //             break;
  //         }
  //       }

  //       auto object = GameObject::createGameObject();
  //       switch (map[0][i + j * static_cast<int>(line)]) {
  //         case '1':
  //           object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
  //               device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::LOST);
  //           gameInterface->push_back(std::move(object));
  //           break;
  //         case 'R':
  //           object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
  //               device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::RED);
  //           gameInterface->push_back(std::move(object));
  //           break;
  //         case 'G':
  //           object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
  //               device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::GREEN);
  //           gameInterface->push_back(std::move(object));
  //           break;
  //         case 'B':
  //           object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
  //               device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::BLUE);
  //           gameInterface->push_back(std::move(object));
  //           break;
  //         default:
  //           break;
  //       }
  //       inc++;
  //     }
  //   }
  //   saveInitialState(*gameInterface);
  // }

  void InterfaceModel::saveInitialState(std::vector<GameObject> &gameInterface) {
    for (const auto &obj : gameInterface) {
      initialState_.push_back(std::make_pair(obj.textureRenderSystem->getIndexTexture(),
                                             obj.textureRenderSystem->getColor()));
    }
  }

  void InterfaceModel::resetToInitialState(std::vector<GameObject> *gameInterface) {
    for (int i = 0; i < static_cast<int>(gameInterface->size()); i++) {
      (*gameInterface)[i].textureRenderSystem->setIndexTexture(initialState_[i].first);
      (*gameInterface)[i].textureRenderSystem->setColor(initialState_[i].second);
    }
  }
}  // namespace ve