#include "interface_model.hpp"

#include <iostream>
#include <utility>
#include <vector>

#include "exit_render_system.hpp"
#include "game_object.hpp"
#include "parsing.hpp"
#include "renderer.hpp"
#include "utils.hpp"

namespace ve {

  InterfaceModel::InterfaceModel(
      Device &device, Renderer &renderer, std::string &lvlPath,
      std::vector<std::shared_ptr<Texture>> &texture, std::vector<std::shared_ptr<Texture>> &exit,
      std::vector<GameObject> &menuInterface, std::vector<std::vector<GameObject>> &playerInterface,
      std::vector<GameObject> &gameInterface, std::vector<GameObject> &displayInterface,
      std::vector<GameObject> &timeInterface, std::vector<GameObject> &menuStartInterface,
      std::vector<GameObject> &exitInterface)
      : device_(device),
        renderer_(renderer),
        gameInterface_(gameInterface),
        menuInterface_(menuInterface),
        playerInterface_(playerInterface),
        displayInterface_(displayInterface),
        timeInterface_(timeInterface),
        menuStartInterface_(menuStartInterface),
        exitInterface_(exitInterface),
        texture_(texture),
        exit_(exit) {
    Parsing parsing(lvlPath);
    xStart_ = parsing.getXStart();
    yStart_ = parsing.getYStart();
    lineSize_ = parsing.getLineSize();
    coloneSize_ = parsing.getColoneSize();
    map_ = parsing.getMap();
  }

  InterfaceModel::~InterfaceModel() {}

  void InterfaceModel::loadTexture() {
    texture_ = {std::make_unique<Texture>(device_, "texture/42Game/LeftArrow.png"),
                std::make_unique<Texture>(device_, "texture/42Game/UpArrow.png"),
                std::make_unique<Texture>(device_, "texture/42Game/RightArrow.png"),
                std::make_unique<Texture>(device_, "texture/42Game/F0.png"),
                std::make_unique<Texture>(device_, "texture/42Game/F1.png"),
                std::make_unique<Texture>(device_, "texture/42Game/F2.png"),
                std::make_unique<Texture>(device_, "texture/42Game/RedBrush.png"),
                std::make_unique<Texture>(device_, "texture/42Game/GreenBrush.png"),
                std::make_unique<Texture>(device_, "texture/42Game/BlueBrush.png"),
                std::make_unique<Texture>(device_, "texture/42Game/Red.png"),
                std::make_unique<Texture>(device_, "texture/42Game/Green.png"),
                std::make_unique<Texture>(device_, "texture/42Game/Blue.png"),
                std::make_unique<Texture>(device_, "texture/42Game/PlayerUp.png"),
                std::make_unique<Texture>(device_, "texture/42Game/Star.png"),
                std::make_unique<Texture>(device_, "texture/42Game/White.png"),
                std::make_unique<Texture>(device_, "texture/42Game/Play.png"),
                std::make_unique<Texture>(device_, "texture/42Game/StepByStep.png"),
                std::make_unique<Texture>(device_, "texture/42Game/Stop.png"),
                std::make_unique<Texture>(device_, "texture/42Game/hello-friend.jpg"),
                std::make_unique<Texture>(device_, "texture/42Game/PlayerDown.png"),
                std::make_unique<Texture>(device_, "texture/42Game/PlayerRight.png"),
                std::make_unique<Texture>(device_, "texture/42Game/PlayerLeft.png"),
                std::make_unique<Texture>(device_, "texture/42Game/Sovietspaceposters.png"),
                std::make_unique<Texture>(device_, "texture/42Game/Start3.png"),
                std::make_unique<Texture>(device_, "texture/42Game/AGameBy.png")};

    exit_ = {std::make_unique<Texture>(device_, "texture/Exit/orig_assets/home.png"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/firstpage.png"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/secondlevela.PNG"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/thirdlevela.PNG"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/fourthlevela.PNG"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/fifthlevela.PNG"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/sixthlevela.PNG"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/finalevela.PNG"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/secondlevelb.PNG"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/thirdlevelb.PNG"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/finalevelb.jpg"),
             std::make_unique<Texture>(device_, "texture/Exit/orig_assets/start.jpg")

    };
  }

  void InterfaceModel::createStartInterface() {
    // Create the game start
    TextureRenderSystem::Builder builder;
    // STARTBUTTON should be at Index 0
    builder = {{{
                    {-0.15, -0.2},
                    {0.0F, 0.0F},
                },
                {
                    {0.15, -0.2},
                    {1.0F, 0.0F},
                },
                {
                    {0.15, -0.1},
                    {1.0F, 1.0F},
                },
                {
                    {-0.15, -0.1},
                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    auto object = GameObject::createGameObject();
    object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder, TextureIndex::STARTBUTTON);
    menuStartInterface_.push_back(std::move(object));

    builder = {{{
                    {-0.25, 0.85},
                    {0.0F, 0.0F},
                },
                {
                    {0.25, 0.85},
                    {1.0F, 0.0F},
                },
                {
                    {0.25, 1.10},
                    {1.0F, 1.0F},
                },
                {
                    {-0.25, 1.10},
                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    object = GameObject::createGameObject();
    object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder, TextureIndex::AGAMEBY);
    menuStartInterface_.push_back(std::move(object));

    builder = {{{
                    {-0.15, -0.075},
                    {0.0F, 0.0F},
                },
                {
                    {0.15, -0.075},
                    {1.0F, 0.0F},
                },
                {
                    {0.15, 0.025},
                    {1.0F, 1.0F},
                },
                {
                    {-0.15, 0.025},
                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    object = GameObject::createGameObject();
    object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder, TextureIndex::STARTBUTTON);
    menuStartInterface_.push_back(std::move(object));

    builder = {{{
                    {-0.15, 0.05},
                    {0.0F, 0.0F},
                },
                {
                    {0.15, 0.05},
                    {1.0F, 0.0F},
                },
                {
                    {0.15, 0.15},
                    {1.0F, 1.0F},
                },
                {
                    {-0.15, 0.15},
                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    object = GameObject::createGameObject();
    object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder, TextureIndex::STARTBUTTON);
    menuStartInterface_.push_back(std::move(object));

    builder = {{{
                    {-0.15, 0.175},
                    {0.0F, 0.0F},
                },
                {
                    {0.15, 0.175},
                    {1.0F, 0.0F},
                },
                {
                    {0.15, 0.275},
                    {1.0F, 1.0F},
                },
                {
                    {-0.15, 0.275},
                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    object = GameObject::createGameObject();
    object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder, TextureIndex::STARTBUTTON);
    menuStartInterface_.push_back(std::move(object));

    builder = {{{
                    {-0.22, -0.175},
                    {0.0F, 0.0F},
                },
                {
                    {-0.17, -0.175},
                    {1.0F, 0.0F},
                },
                {
                    {-0.17, -0.125},
                    {1.0F, 1.0F},
                },
                {
                    {-0.22, -0.125},
                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    object = GameObject::createGameObject();
    object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder, TextureIndex::PLAYERRIGHT);
    menuStartInterface_.push_back(std::move(object));

    builder = {{{
                    {-0.22, -0.125},
                    {0.0F, 0.0F},
                },
                {
                    {-0.17, -0.125},
                    {1.0F, 0.0F},
                },
                {
                    {-0.17, -0.075},
                    {1.0F, 1.0F},
                },
                {
                    {-0.22, -0.075},
                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    object = GameObject::createGameObject();
    object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder, TextureIndex::DONOTSHOW);
    menuStartInterface_.push_back(std::move(object));

    builder = {{{
                    {-1, -1},
                    {0.0F, 0.0F},
                },
                {
                    {1, -1},
                    {1.0F, 0.0F},
                },
                {
                    {1, 1},
                    {1.0F, 1.0F},
                },
                {
                    {-1, 1},
                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    object = GameObject::createGameObject();
    object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder, TextureIndex::BACKGROUNDSTART);
    menuStartInterface_.push_back(std::move(object));
  }

  void InterfaceModel::createMenuInterface() {
    TextureRenderSystem::Builder builder;
    float xStart = -0.95;
    float yStart = 0.80F;
    int squareNumbers = 3;

    int mapLineSize = static_cast<int>(map_[2].find('\n'));

    // create menu
    for (int j = 0; j < squareNumbers; j++) {
      for (int i = 0; i < squareNumbers; i++) {
        builder = {{{{xStart + (static_cast<float>(i) * WIDTHVERTEX),
                      yStart + (static_cast<float>(j) * HEIGHTVERTEX)},

                     {0.0F, 0.0F}},
                    {{xStart + WIDTHVERTEX + (static_cast<float>(i) * WIDTHVERTEX),
                      yStart + (static_cast<float>(j) * HEIGHTVERTEX)},

                     {1.0F, 0.0F}},
                    {{xStart + WIDTHVERTEX + (static_cast<float>(i) * WIDTHVERTEX),
                      yStart + HEIGHTVERTEX + (static_cast<float>(j) * HEIGHTVERTEX)},

                     {1.0F, 1.0F}},
                    {{xStart + (static_cast<float>(i) * WIDTHVERTEX),
                      yStart + HEIGHTVERTEX + (static_cast<float>(j) * HEIGHTVERTEX)},

                     {0.0F, 1.0F}}},
                   {0, 1, 2, 2, 3, 0}};
        auto object = GameObject::createGameObject();
        object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderer_, texture_, builder,
            static_cast<TextureIndex>(i + (j * squareNumbers)));
        setAlphaColor(object, i + (j * (mapLineSize + 1)));
        menuInterface_.push_back(std::move(object));
      }
    }
    // create color square
    for (int j = 0; j < squareNumbers; j++) {
      builder = {
          {{{xStart + 0.01F + (3 * WIDTHVERTEX), yStart + (static_cast<float>(j) * HEIGHTVERTEX)},

            {1.0F, 0.0F}},
           {{xStart + WIDTHVERTEX + 0.01F + (3 * WIDTHVERTEX),
             yStart + (static_cast<float>(j) * HEIGHTVERTEX)},

            {0.0F, 0.0F}},
           {{xStart + WIDTHVERTEX + 0.01F + (3 * WIDTHVERTEX),
             yStart + HEIGHTVERTEX + (static_cast<float>(j) * HEIGHTVERTEX)},

            {0.0F, 1.0F}},
           {{xStart + 0.01F + (3 * WIDTHVERTEX),
             yStart + HEIGHTVERTEX + (static_cast<float>(j) * HEIGHTVERTEX)},

            {1.0F, 1.0F}}},
          {0, 1, 2, 2, 3, 0}};
      auto object = GameObject::createGameObject();
      object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
          device_, renderer_, texture_, builder,
          static_cast<TextureIndex>(j + (squareNumbers * squareNumbers)));
      setAlphaColor(object, mapLineSize + ((mapLineSize + 1) * j));
      menuInterface_.push_back(std::move(object));
    }
    // create Button play stop and pause
    for (int i = 0; i < squareNumbers; i++) {
      builder = {{{{xStart + (static_cast<float>(i) * WIDTHVERTEX), yStart - 0.06},

                   {0.0F, 0.0F}},
                  {
                      {xStart + (static_cast<float>(i) * WIDTHVERTEX) + WIDTHVERTEX, yStart - 0.06},

                      {1.0F, 0.0F},
                  },
                  {{xStart + (static_cast<float>(i) * WIDTHVERTEX) + WIDTHVERTEX, yStart - 0.01},

                   {1.0F, 1.0F}},
                  {{xStart + (static_cast<float>(i) * WIDTHVERTEX), yStart - 0.01},

                   {0.0F, 1.0F}}},
                 {0, 1, 2, 2, 3, 0}};

      auto object = GameObject::createGameObject();
      object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
          device_, renderer_, texture_, builder, static_cast<TextureIndex>(i + TextureIndex::PLAY));
      menuInterface_.push_back(std::move(object));
    }
  }

  void InterfaceModel::createTimeInterface() {
    TextureRenderSystem::Builder builder;
    float xStart = -1;
    float yStart = -0.8F;
    int displayNumbers = 100;

    for (int i = 0; i < displayNumbers; i++) {
      builder = {{{{xStart + (static_cast<float>(i) * 0.02), yStart - 0.03},

                   {0.0F, 0.0F}},
                  {
                      {xStart + (static_cast<float>(i) * 0.02) + 0.02, yStart - 0.03},

                      {1.0F, 0.0F},
                  },
                  {{xStart + (static_cast<float>(i) * 0.02) + 0.02, yStart - 0.01},

                   {1.0F, 1.0F}},
                  {{xStart + (static_cast<float>(i) * 0.02), yStart - 0.01},

                   {0.0F, 1.0F}}},
                 {0, 1, 2, 2, 3, 0}};
      auto object = GameObject::createGameObject();
      object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
          device_, renderer_, texture_, builder, static_cast<TextureIndex>(TextureIndex::WHITE));

      timeInterface_.push_back(std::move(object));
    }
  }

  void InterfaceModel::createDisplayInterface() {  // create display
    float xStart = -0.75;
    int displayNumbers = 30;
    float yStart = 0.72F;
    TextureRenderSystem::Builder builder;
    for (int i = 0; i < displayNumbers; i++) {
      builder = {{{{xStart + (static_cast<float>(i) * WIDTHVERTEX), yStart - 0.06},

                   {0.0F, 0.0F}},
                  {
                      {xStart + (static_cast<float>(i) * WIDTHVERTEX) + WIDTHVERTEX, yStart - 0.06},

                      {1.0F, 0.0F},
                  },
                  {{xStart + (static_cast<float>(i) * WIDTHVERTEX) + WIDTHVERTEX, yStart - 0.01},

                   {1.0F, 1.0F}},
                  {{xStart + (static_cast<float>(i) * WIDTHVERTEX), yStart - 0.01},

                   {0.0F, 1.0F}}},
                 {0, 1, 2, 2, 3, 0}};
      auto object = GameObject::createGameObject();
      object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
          device_, renderer_, texture_, builder, static_cast<TextureIndex>(TextureIndex::WHITE));
      displayInterface_.push_back(std::move(object));
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
            {{{{xStart + (static_cast<float>(i) * 0.05F), yStart + (static_cast<float>(j) * 0.05)},

               {0.0F, 0.0F}},
              {{xStart + 0.05F + (static_cast<float>(i) * 0.05F),
                yStart + (static_cast<float>(j) * 0.05)},

               {1.0F, 0.0F}},
              {{xStart + 0.05F + (static_cast<float>(i) * 0.05F),
                yStart + 0.05F + (static_cast<float>(j) * 0.05)},

               {1.0F, 1.0F}},
              {{xStart + (static_cast<float>(i) * 0.05F),
                yStart + 0.05F + (static_cast<float>(j) * 0.05)},

               {0.0F, 1.0F}}},
             {0, 1, 2, 2, 3, 0}});
        auto object = GameObject::createGameObject();
        object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderer_, texture_, builder[inc++], TextureIndex::WHITE);
        playerInterface_[j].push_back(std::move(object));
      }
    }

    auto SquareF0 = GameObject::createGameObject();
    SquareF0.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder[0], TextureIndex::F0);
    menuInterface_.push_back(std::move(SquareF0));
    playerInterface_[0].erase(playerInterface_[0].begin());

    auto SquareF1 = GameObject::createGameObject();
    SquareF1.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder[sizeF[0]], TextureIndex::F1);

    if (sizeF[1] == 1) {
      SquareF1.textureRenderSystem->setColor(glm::vec4(1.0, 1.0, 1.0, 0.4));
    }
    menuInterface_.push_back(std::move(SquareF1));
    playerInterface_[1].erase(playerInterface_[1].begin());

    auto SquareF2 = GameObject::createGameObject();
    SquareF2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderer_, texture_, builder[sizeF[0] + sizeF[1]], TextureIndex::F2);
    if (sizeF[2] == 1) {
      SquareF2.textureRenderSystem->setColor(glm::vec4(1.0, 1.0, 1.0, 0.4));
    }
    menuInterface_.push_back(std::move(SquareF2));
    playerInterface_[2].erase(playerInterface_[2].begin());
  }

  void InterfaceModel::createGameMap() {
    TextureRenderSystem::Builder builder;
    for (int j = 0; j < coloneSize_; j++) {
      for (int i = 0; i < lineSize_; i++) {
        builder = {{{
                        {xStart_ + (static_cast<float>(i) * 0.05F),
                         yStart_ + (static_cast<float>(j) * 0.05)},

                        {0.0F, 0.0F},
                    },
                    {
                        {xStart_ + 0.05F + (static_cast<float>(i) * 0.05F),
                         yStart_ + (static_cast<float>(j) * 0.05)},

                        {1.0F, 0.0F},
                    },
                    {
                        {xStart_ + 0.05F + (static_cast<float>(i) * 0.05F),
                         yStart_ + 0.05F + (static_cast<float>(j) * 0.05)},

                        {1.0F, 1.0F},
                    },
                    {
                        {xStart_ + (static_cast<float>(i) * 0.05F),
                         yStart_ + 0.05F + (static_cast<float>(j) * 0.05)},

                        {0.0F, 1.0F},
                    }},
                   {0, 1, 2, 2, 3, 0}};
        if (map_[0][i + (j * lineSize_)] != 1) {
          switch (map_[1][i + (j * lineSize_)]) {
            case 'V': {
              auto object2 = GameObject::createGameObject();
              object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                  device_, renderer_, texture_, builder, TextureIndex::STAR);
              gameInterface_.push_back(std::move(object2));
              countStarStart_++;
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

          switch (map_[0][i + (j * lineSize_)]) {
            case 'R': {
              auto object = GameObject::createGameObject();
              object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                  device_, renderer_, texture_, builder, TextureIndex::RED);
              object.textureRenderSystem->setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
              gameInterface_.push_back(std::move(object));
              break;
            }
            case 'G': {
              auto object = GameObject::createGameObject();
              object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                  device_, renderer_, texture_, builder, TextureIndex::GREEN);
              object.textureRenderSystem->setColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
              gameInterface_.push_back(std::move(object));
              break;
            }
            case 'B': {
              auto object = GameObject::createGameObject();
              object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                  device_, renderer_, texture_, builder, TextureIndex::BLUE);
              object.textureRenderSystem->setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
              gameInterface_.push_back(std::move(object));
              break;
            }
            default:
              break;
          }
        }
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
            device_, renderer_, texture_, builder, TextureIndex::PLAYERUP);
        gameInterface_.emplace(gameInterface_.begin(), std::move(object2));
        break;
      }
      case 'E': {
        playerStart_.Angle = 0.0F;
        object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderer_, texture_, builder, TextureIndex::PLAYERRIGHT);
        gameInterface_.emplace(gameInterface_.begin(), std::move(object2));
        break;
      }
      case 'W': {
        playerStart_.Angle = 180.0F;
        object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderer_, texture_, builder, TextureIndex::PLAYERLEFT);
        gameInterface_.emplace(gameInterface_.begin(), std::move(object2));
        break;
      }
      case 'S': {
        playerStart_.Angle = 270.0F;
        object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
            device_, renderer_, texture_, builder, TextureIndex::PLAYERDOWN);
        gameInterface_.emplace(gameInterface_.begin(), std::move(object2));
        break;
      }
      default: {
        std::cerr << "Error: Invalid player orientation character '" << c << "'." << '\n';
        return;
      }
    }
  }

  void InterfaceModel::uptateGameLvl(std::string &lvlPath) {
    initialState_.clear();
    countStarStart_ = 0;
    Parsing parsing(lvlPath);
    xStart_ = parsing.getXStart();
    yStart_ = parsing.getYStart();
    lineSize_ = parsing.getLineSize();
    coloneSize_ = parsing.getColoneSize();
    map_ = parsing.getMap();
  }

  void InterfaceModel::setAlphaColor(GameObject &obj, int indexMap) {
    if (map_[2][indexMap] == '0') {
      obj.textureRenderSystem->setColor(glm::vec4(1.0, 1.0, 1.0, 0.4));
    }
  }

  void InterfaceModel::exitGame() {
    ExitRenderSystem::Builder builder;
    builder = {{{
                    {-1, -1},

                    {0.0F, 0.0F},
                },
                {
                    {1, -1},

                    {1.0F, 0.0F},
                },
                {
                    {1, 1},

                    {1.0F, 1.0F},
                },
                {
                    {-1, 1},

                    {0.0F, 1.0F},
                }},
               {0, 1, 2, 2, 3, 0}};
    auto object = GameObject::createGameObject();
    object.exitRenderSystem
        = std::make_unique<ExitRenderSystem>(device_, renderer_, exit_, builder, ExitIndex::HOME);
    exitInterface_.push_back(std::move(object));
  }
}  // namespace ve