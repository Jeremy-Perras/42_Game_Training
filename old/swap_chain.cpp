#include "swap_chain.hpp"

#include <_types/_uint32_t.h>

namespace ve {
  SwapChain::SwapChain(VkExtent2D windowExtent, Device &device)
      : _windowExtent{windowExtent}, _device(device) {
    createSwapChain();
    createImageViews();
    createRenderPass();
    createFramebuffers();
  };

  SwapChain::~SwapChain() {
    vkDestroySemaphore(_device.getDevice(), _imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(_device.getDevice(), _renderFinishedSemaphore, nullptr);
    vkDestroyFence(_device.getDevice(), _inFlightFence, nullptr);
    for (auto *framebuffer : _swapChainFramebuffers) {
      vkDestroyFramebuffer(_device.getDevice(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(_device.getDevice(), _renderPass, nullptr);
    for (auto *imageView : _swapChainImageViews) {
      vkDestroyImageView(_device.getDevice(), imageView, nullptr);
    }
    vkDestroySwapchainKHR(_device.getDevice(), _swapChain, nullptr);
  };

  void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport
        = _device.querySwapChainSupport(_device.getPhysicalDevice());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0
        && imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _device.getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = _device.findQueueFamilies(_device.getPhysicalDevice());
    if (indices.graphicsFamily.has_value() && indices.presentFamily.has_value()) {
      uint32_t queueFamilyIndices[]
          = {indices.graphicsFamily.value(), indices.presentFamily.value()};

      if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
      } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optionnel
        createInfo.pQueueFamilyIndices = nullptr;  // Optionnel
      }
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(_device.getDevice(), &createInfo, nullptr, &_swapChain)
        != VK_SUCCESS) {
      throw std::runtime_error("échec de la création de la swap chain!");
    }
    vkGetSwapchainImagesKHR(_device.getDevice(), _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_device.getDevice(), _swapChain, &imageCount, _swapChainImages.data());

    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;
  }

  VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
          && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
      }
    }
    return availableFormats[0];
  }

  VkPresentModeKHR SwapChain::chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return availablePresentMode;
      }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    }
    VkExtent2D actualExtent = _windowExtent;

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                    capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                     capabilities.maxImageExtent.height);

    return actualExtent;
  }

  void SwapChain::createImageViews() {
    _swapChainImageViews.resize(_swapChainImages.size());
    for (size_t i = 0; i < _swapChainImages.size(); i++) {
      VkImageViewCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      createInfo.image = _swapChainImages[i];
      createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      createInfo.format = _swapChainImageFormat;
      createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;
      if (vkCreateImageView(_device.getDevice(), &createInfo, nullptr, &_swapChainImageViews[i])
          != VK_SUCCESS) {
        throw std::runtime_error("échec de la création d'une image view!");
      }
    }
  }
  void SwapChain::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(_device.getDevice(), &renderPassInfo, nullptr, &_renderPass)
        != VK_SUCCESS) {
      throw std::runtime_error("échec de la création de la render pass!");
    }
  }

  void SwapChain::createFramebuffers() {
    _swapChainFramebuffers.resize(_swapChainImageViews.size());
    for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
      VkImageView attachments[] = {_swapChainImageViews[i]};

      VkFramebufferCreateInfo framebufferInfo{};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = _renderPass;
      framebufferInfo.attachmentCount = 1;
      framebufferInfo.pAttachments = attachments;
      framebufferInfo.width = _swapChainExtent.width;
      framebufferInfo.height = _swapChainExtent.height;
      framebufferInfo.layers = 1;

      if (vkCreateFramebuffer(_device.getDevice(), &framebufferInfo, nullptr,
                              &_swapChainFramebuffers[i])
          != VK_SUCCESS) {
        throw std::runtime_error("échec de la création d'un framebuffer!");
      }
    }
  }

  void SwapChain::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateSemaphore(_device.getDevice(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore)
            != VK_SUCCESS
        || vkCreateSemaphore(_device.getDevice(), &semaphoreInfo, nullptr,
                             &_renderFinishedSemaphore)
               != VK_SUCCESS
        || vkCreateFence(_device.getDevice(), &fenceInfo, nullptr, &_inFlightFence) != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores!");
    }
  }

  void SwapChain::drawFrame() {
    vkWaitForFences(_device.getDevice(), 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(_device.getDevice(), 1, &_inFlightFence);
    uint32_t imageIndex;
    vkAcquireNextImageKHR(_device.getDevice(), _swapChain, UINT64_MAX, _imageAvailableSemaphore,
                          VK_NULL_HANDLE, &imageIndex);

    // vkResetCommandBuffer(commandBuffer, 0);
  }
}  // namespace ve