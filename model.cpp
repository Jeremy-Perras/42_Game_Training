#include "model.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>

#include "device.hpp"
#include "render_system.hpp"
#include "renderer.hpp"
#include "swap_chain.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {

  Model::Model(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorLayout)
      : device_(device), renderPass_(renderPass), descriptorLayout_(descriptorLayout) {}

  Model::~Model() {}

  std::vector<RenderSystem::Vertex> Model::createSquareModel() {
    std::vector<RenderSystem::Vertex> vertices = {
        {{-1.0F, 0.70F}, {1.0F, 1.0F, 1.0F}}, {{1.0F, 0.70F}, {1.0F, 1.0F, 1.0F}},
        {{1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}},   {{-1.0F, 0.70F}, {1.0F, 1.0F, 1.0F}},
        {{-1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}},  {{1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}},  //
    };
    return vertices;
  }

  void Model::loadTexture(std::vector<std::shared_ptr<Texture>> *texture) {
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

  void Model::menuInterface(std::vector<GameObject> *menuInterface) {
    std::vector<TextureRenderSystem::Builder> builder;
    float xStart = -0.95;
    float yStart = 0.80F;
    for (int j = 0; j < 3; j++) {
      for (int i = 0; i < 3; i++) {
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
      }
    }

    for (int j = 0; j < 3; j++) {
      builder.push_back(
          {{{{xStart + 0.01F + 3 * 0.05F, yStart + static_cast<float>(j) * 0.05},

             {1.0F, 0.0F},
             {1.0, 1.0, 1.0, 1.0}},
            {{xStart + 0.05F + 0.01F + 3 * 0.05F, yStart + static_cast<float>(j) * 0.05},

             {0.0F, 0.0F},
             {1.0, 1.0, 1.0, 1.0}},
            {{xStart + 0.05F + 0.01F + 3 * 0.05F, yStart + 0.05F + static_cast<float>(j) * 0.05},

             {0.0F, 1.0F},
             {1.0, 1.0, 1.0, 1.0}},
            {{xStart + 0.01F + 3 * 0.05F, yStart + 0.05F + static_cast<float>(j) * 0.05},

             {1.0F, 1.0F},
             {1.0, 1.0, 1.0, 1.0}}},
           {0, 1, 2, 2, 3, 0}});
    }

    for (int i = 0; i < 3; i++) {
      builder.push_back({{{{xStart + i * 0.05, yStart - 0.06},

                           {1.0F, 0.0F},
                           {1.0, 1.0, 1.0, 1.0}},
                          {{xStart + i * 0.05 + 0.05F, yStart - 0.06},

                           {0.0F, 0.0F},
                           {1.0, 1.0, 1.0, 1.0}},
                          {{xStart + i * 0.05 + 0.05, yStart - 0.01},

                           {0.0F, 1.0F},
                           {1.0, 1.0, 1.0, 1.0}},
                          {{xStart + i * 0.05, yStart - 0.01},

                           {1.0F, 1.0F},
                           {1.0, 1.0, 1.0, 1.0}}},
                         {0, 1, 2, 2, 3, 0}});
    }

    auto squareMenuTop = GameObject::createGameObject();
    auto squareMenuTop2 = GameObject::createGameObject();
    auto squareMenuTop3 = GameObject::createGameObject();
    auto squareMenuTop4 = GameObject::createGameObject();
    squareMenuTop.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[0], TextureIndex::ARROWLEFT);
    squareMenuTop2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[1], TextureIndex::ARROWUP);
    squareMenuTop3.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[2], TextureIndex::ARROWRIGHT);
    squareMenuTop4.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[9], TextureIndex::RED);
    squareMenuTop4.textureRenderSystem->setColor({1.0, 0.0, 0.0, 1.0});
    menuInterface->push_back(std::move(squareMenuTop));
    menuInterface->push_back(std::move(squareMenuTop2));
    menuInterface->push_back(std::move(squareMenuTop3));
    menuInterface->push_back(std::move(squareMenuTop4));

    auto squareMenuMiddle = GameObject::createGameObject();
    auto squareMenuMiddle2 = GameObject::createGameObject();
    auto squareMenuMiddle3 = GameObject::createGameObject();
    auto squareMenuMiddle4 = GameObject::createGameObject();
    squareMenuMiddle.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[3], TextureIndex::F0);
    squareMenuMiddle2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[4], TextureIndex::F1);
    squareMenuMiddle3.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[5], TextureIndex::F2);
    squareMenuMiddle4.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[10], TextureIndex::GREEN);
    squareMenuMiddle4.textureRenderSystem->setColor({0.0, 1.0, 0.0, 1.0});
    menuInterface->push_back(std::move(squareMenuMiddle));
    menuInterface->push_back(std::move(squareMenuMiddle2));
    menuInterface->push_back(std::move(squareMenuMiddle3));
    menuInterface->push_back(std::move(squareMenuMiddle4));

    auto squareMenuBack = GameObject::createGameObject();
    auto squareMenuBack2 = GameObject::createGameObject();
    auto squareMenuBack3 = GameObject::createGameObject();
    auto squareMenuBack4 = GameObject::createGameObject();
    squareMenuBack.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[6], TextureIndex::BRUSHRED);
    squareMenuBack2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[7], TextureIndex::BRUSHGREEN);
    squareMenuBack3.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[8], TextureIndex::BRUSHBLUE);
    squareMenuBack4.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[11], TextureIndex::BLUE);

    squareMenuBack4.textureRenderSystem->setColor({0.0, 0.0, 1.0, 1.0});
    menuInterface->push_back(std::move(squareMenuBack));
    menuInterface->push_back(std::move(squareMenuBack2));
    menuInterface->push_back(std::move(squareMenuBack3));
    menuInterface->push_back(std::move(squareMenuBack4));

    auto play = GameObject::createGameObject();
    auto pause = GameObject::createGameObject();
    auto stop = GameObject::createGameObject();
    play.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[12], TextureIndex::PLAY);
    pause.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[13], TextureIndex::PAUSE);
    stop.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass_, descriptorLayout_, builder[14], TextureIndex::STOP);
    menuInterface->push_back(std::move(play));
    menuInterface->push_back(std::move(pause));
    menuInterface->push_back(std::move(stop));
  }

  void Model::playerInterface(std::vector<std::vector<GameObject>> *playerInterface,
                              std::vector<GameObject> *menuInterface, interfaceSize size) {
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
    renderBuilder.vertices = Model::createSquareModel();
    auto whiteSquare = GameObject::createGameObject();
    whiteSquare.renderSystem
        = std::make_unique<RenderSystem>(device_, renderPass_, descriptorLayout_, renderBuilder);
    menuInterface->push_back(std::move(whiteSquare));
  }

}  // namespace ve