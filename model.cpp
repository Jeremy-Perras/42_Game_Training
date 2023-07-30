#include "model.hpp"

#include <cstddef>
#include <cstring>
#include <memory>

#include "device.hpp"
#include "render_system.hpp"
#include "renderer.hpp"
#include "swap_chain.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {

  Model::Model(Device &device) : device_(device) {}

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

    texture->push_back(std::move(arrowLeft));
    texture->push_back(std::move(f0));
    texture->push_back(std::move(red));
    texture->push_back(std::move(arrowUp));
    texture->push_back(std::move(f1));
    texture->push_back(std::move(blue));
    texture->push_back(std::move(arrowRight));
    texture->push_back(std::move(f2));
    texture->push_back(std::move(green));
    texture->push_back(std::move(rocket));
  }

  void Model::createMenu(std::vector<GameObject> *gameObjects, VkRenderPass renderPass,
                         VkDescriptorSetLayout descriptorLayout) {
    float xStart = -0.95;
    float yStart = 0.80F;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        builder_.push_back(
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
      }
    }

    for (int j = 0; j < 3; j++) {
      builder_.push_back(
          {{{{xStart + 0.01F + 3 * 0.05F, yStart + static_cast<float>(j) * 0.05},

             {1.0F, 0.0F}},
            {{xStart + 0.05F + 0.01F + 3 * 0.05F, yStart + static_cast<float>(j) * 0.05},

             {0.0F, 0.0F}},
            {{xStart + 0.05F + 0.01F + 3 * 0.05F, yStart + 0.05F + static_cast<float>(j) * 0.05},

             {0.0F, 1.0F}},
            {{xStart + 0.01F + 3 * 0.05F, yStart + 0.05F + static_cast<float>(j) * 0.05},

             {1.0F, 1.0F}}},
           {0, 1, 2, 2, 3, 0}});
    }

    builder_.push_back({{{{0.0, 0.0}, {1.0F, 0.0F}},
                         {{0.05, 0.0}, {0.0F, 0.0F}},
                         {{0.05, 0.05}, {0.0F, 1.0F}},
                         {{0.0, 0.05}, {1.0F, 1.0F}}},
                        {0, 1, 2, 2, 3, 0}});
    auto squareMenuTop = GameObject::createGameObject();
    auto squareMenuTop2 = GameObject::createGameObject();
    auto squareMenuTop3 = GameObject::createGameObject();
    auto squareMenuTop4 = GameObject::createGameObject();
    squareMenuTop.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[0], 0);
    squareMenuTop2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[1], 1);
    squareMenuTop3.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[2], 2);
    squareMenuTop4.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[9], 100);
    gameObjects->push_back(std::move(squareMenuTop));
    gameObjects->push_back(std::move(squareMenuTop2));
    gameObjects->push_back(std::move(squareMenuTop3));
    gameObjects->push_back(std::move(squareMenuTop4));

    auto squareMenuMiddle = GameObject::createGameObject();
    auto squareMenuMiddle2 = GameObject::createGameObject();
    auto squareMenuMiddle3 = GameObject::createGameObject();
    auto squareMenuMiddle4 = GameObject::createGameObject();
    squareMenuMiddle.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[3], 3);
    squareMenuMiddle2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[4], 4);
    squareMenuMiddle3.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[5], 5);
    squareMenuMiddle4.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[10], 101);
    gameObjects->push_back(std::move(squareMenuMiddle));
    gameObjects->push_back(std::move(squareMenuMiddle2));
    gameObjects->push_back(std::move(squareMenuMiddle3));
    gameObjects->push_back(std::move(squareMenuMiddle4));

    auto squareMenuBack = GameObject::createGameObject();
    auto squareMenuBack2 = GameObject::createGameObject();
    auto squareMenuBack3 = GameObject::createGameObject();
    auto squareMenuBack4 = GameObject::createGameObject();
    squareMenuBack.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[6], 6);
    squareMenuBack2.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[7], 7);
    squareMenuBack3.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[8], 8);
    squareMenuBack4.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[11], 102);
    auto rocket = GameObject::createGameObject();
    rocket.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[builder_.size() - 1], 9);
    auto test = GameObject::createGameObject();
    test.textureRenderSystem = std::make_unique<TextureRenderSystem>(
        device_, renderPass, descriptorLayout, builder_[builder_.size() - 1], 8);
    gameObjects->push_back(std::move(squareMenuBack));
    gameObjects->push_back(std::move(squareMenuBack2));
    gameObjects->push_back(std::move(squareMenuBack3));
    gameObjects->push_back(std::move(squareMenuBack4));
    RenderSystem::Builder renderBuilder;
    renderBuilder.vertices = Model::createSquareModel();
    auto whiteSquare = GameObject::createGameObject();
    whiteSquare.renderSystem
        = std::make_unique<RenderSystem>(device_, renderPass, descriptorLayout, renderBuilder);
    gameObjects->push_back(std::move(whiteSquare));

    gameObjects->push_back(std::move(rocket));
    gameObjects->push_back(std::move(test));
  }

}  // namespace ve