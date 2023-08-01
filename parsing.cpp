#include "parsing.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "game_object.hpp"

namespace ve {
  Parsing::Parsing(const std ::string &filepath, Device &device, VkRenderPass renderPass,
                   VkDescriptorSetLayout descriptorLayout)
      : device_(device), renderPass_(renderPass), descriptorLayout_(descriptorLayout) {
    initParsing(filepath);
  }

  void Parsing::initParsing(const std::string &filepath) {
    std::ifstream file(filepath);
    std::string line;
    int i = 0;

    map_.resize(3);

    if (!file.is_open()) {
      throw std::runtime_error(std::string{"failed to open file:"} + filepath);
    }
    while (file) {
      std::getline(file, line);
      if (line.empty()) {
        i++;
      } else {
        map_[i].append(line);
        map_[i].append("\n");
      }
    }
    file.close();
  }

  void Parsing::createMap(std::vector<GameObject> *gameInterface) {
    unsigned long line = map_[0].substr(0, map_[0].find('\n')).size();
    int lineStart = static_cast<int>(line) / 2;
    int colone = 0;
    int inc = 0;
    std::string::size_type n = 0;

    for (auto c : map_[0]) {
      if (c == '\n') {
        colone++;
      }
    }
    int coloneStart = static_cast<int>(colone) / 2;
    float xStart = -0.05F * static_cast<float>(lineStart);
    float yStart = -0.05F * static_cast<float>(coloneStart);
    std::vector<TextureRenderSystem::Builder> builder;

    while ((n = map_[0].find('\n')) != std::string::npos) {
      map_[0].erase(n, 1);
    }
    n = 0;
    while ((n = map_[1].find('\n')) != std::string::npos) {
      map_[1].erase(n, 1);
    }

    for (int j = 0; j < colone; j++) {
      for (int i = 0; i < static_cast<int>(line); i++) {
        builder.push_back(
            {{{{xStart + static_cast<float>(i) * 0.05F, yStart + static_cast<float>(j) * 0.05},

               {0.0F, 0.0F},
               {1.0, 1.0, 1.0, 1.0}},
              {{xStart + 0.05F + static_cast<float>(i) * 0.05F,
                yStart + static_cast<float>(j) * 0.05},

               {1.0F, 0.0F},
               {1.0, 1.0, 1.0, 1.0}},
              {{xStart + 0.05F + static_cast<float>(i) * 0.05F,
                yStart + 0.05F + static_cast<float>(j) * 0.05},

               {1.0F, 1.0F},
               {1.0, 1.0, 1.0, 1.0}},
              {{xStart + static_cast<float>(i) * 0.05F,
                yStart + 0.05F + static_cast<float>(j) * 0.05},

               {0.0F, 1.0F},
               {1.0, 1.0, 1.0, 1.0}}},
             {0, 1, 2, 2, 3, 0}});

        if (map_[1][i + j * static_cast<int>(line)] != '1') {
          auto object2 = GameObject::createGameObject();
          switch (map_[1][i + j * static_cast<int>(line)]) {
            case 'S':
              countStar_++;
              object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                  device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::STAR);
              gameInterface->push_back(std::move(object2));
              break;
            case 'P':
              playerStart_.x
                  = (builder[inc].vertices[0].pos.x + builder[inc].vertices[1].pos.x) / 2;
              playerStart_.y
                  = (builder[inc].vertices[0].pos.y + builder[inc].vertices[2].pos.y) / 2;

              playerStart_.Angle = 90.0F;
              object2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                  device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::PLAYER);
              gameInterface->emplace(gameInterface->begin(), std::move(object2));
              playerPointer_ = gameInterface->data();

              break;
            default:
              break;
          }
        }

        auto object = GameObject::createGameObject();
        switch (map_[0][i + j * static_cast<int>(line)]) {
          case '1':
            object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::WHITE);

            gameInterface->push_back(std::move(object));
            break;
          case 'R':
            object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::RED);
            gameInterface->push_back(std::move(object));
            break;
          case 'G':
            object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::GREEN);
            gameInterface->push_back(std::move(object));
            break;
          case 'B':
            object.textureRenderSystem = std::make_unique<TextureRenderSystem>(
                device_, renderPass_, descriptorLayout_, builder[inc], TextureIndex::BLUE);
            gameInterface->push_back(std::move(object));
            break;
          default:
            break;
        }
        inc++;
      }
    }
  }

}  // namespace ve