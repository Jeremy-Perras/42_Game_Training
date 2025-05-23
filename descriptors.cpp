#include "descriptors.hpp"

#include <_types/_uint32_t.h>

// std
#include <cassert>
#include <stdexcept>

#include "vulkan/vulkan_core.h"

namespace ve {

  // *************** Descriptor Set Layout Builder *********************

  DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::addBinding(
      uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags,
      uint32_t count) {
    assert(bindings_.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;

    bindings_[binding] = layoutBinding;
    return *this;
  }

  std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
    return std::make_unique<DescriptorSetLayout>(device_, bindings_);
  }

  // *************** Descriptor Set Layout *********************

  DescriptorSetLayout::DescriptorSetLayout(
      Device &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
      : device_{device}, bindings_{bindings} {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    setLayoutBindings.reserve(bindings.size());
    for (auto kv : bindings) {
      setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(device_.getDevice(), &descriptorSetLayoutInfo, nullptr,
                                    &descriptorSetLayout_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor set layout!");
    }
  }

  DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device_.getDevice(), descriptorSetLayout_, nullptr);
  }
  // *************** Descriptor Set Layout Push Builder *********************

  DescriptorSetLayoutPush::Builder &DescriptorSetLayoutPush::Builder::addBinding(
      uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags,
      uint32_t count) {
    assert(bindings_.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;

    bindings_[binding] = layoutBinding;
    return *this;
  }

  std::unique_ptr<DescriptorSetLayoutPush> DescriptorSetLayoutPush::Builder::build() const {
    return std::make_unique<DescriptorSetLayoutPush>(device_, bindings_);
  }

  // *************** Descriptor Set Layout Push *********************

  DescriptorSetLayoutPush::DescriptorSetLayoutPush(
      Device &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
      : device_{device}, bindings_{bindings} {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    setLayoutBindings.reserve(bindings.size());
    for (auto kv : bindings) {
      setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();
    descriptorSetLayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;

    if (vkCreateDescriptorSetLayout(device_.getDevice(), &descriptorSetLayoutInfo, nullptr,
                                    &descriptorSetLayoutPush_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor set layout!");
    }
  }

  DescriptorSetLayoutPush::~DescriptorSetLayoutPush() {
    vkDestroyDescriptorSetLayout(device_.getDevice(), descriptorSetLayoutPush_, nullptr);
  }

  // *************** Descriptor Pool Builder *********************

  DescriptorPool::Builder &DescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType,
                                                                uint32_t count) {
    poolSizes_.push_back({descriptorType, count});
    return *this;
  }

  DescriptorPool::Builder &DescriptorPool::Builder::setPoolFlags(
      VkDescriptorPoolCreateFlags flags) {
    poolFlags_ = flags;
    return *this;
  }
  DescriptorPool::Builder &DescriptorPool::Builder::setMaxSets(uint32_t count) {
    maxSets_ = count;
    return *this;
  }

  std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
    return std::make_unique<DescriptorPool>(device_, maxSets_, poolFlags_, poolSizes_);
  }

  // *************** Descriptor Pool *********************

  DescriptorPool::DescriptorPool(Device &device, uint32_t maxSets,
                                 VkDescriptorPoolCreateFlags poolFlags,
                                 const std::vector<VkDescriptorPoolSize> &poolSizes)
      : device_{device} {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(device_.getDevice(), &descriptorPoolInfo, nullptr, &descriptorPool_)
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor pool!");
    }
  }

  DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(device_.getDevice(), descriptorPool_, nullptr);
  }

  bool DescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout,
                                          VkDescriptorSet &descriptor) const {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool_;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // indexBinding
    // VkDescriptorSetVariableDescriptorCountAllocateInfoEXT variableDescriptorCountAllocInfo = {};

    // uint32_t variableDescCounts[] = {static_cast<uint32_t>(19)};
    // variableDescriptorCountAllocInfo.sType
    //     = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    // variableDescriptorCountAllocInfo.descriptorSetCount = 1;
    // variableDescriptorCountAllocInfo.pDescriptorCounts = variableDescCounts;
    // allocInfo.pNext = &variableDescriptorCountAllocInfo;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(device_.getDevice(), &allocInfo, &descriptor) != VK_SUCCESS) {
      return false;
    }
    return true;
  }

  void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const {
    vkFreeDescriptorSets(device_.getDevice(), descriptorPool_,
                         static_cast<uint32_t>(descriptors.size()), descriptors.data());
  }

  void DescriptorPool::resetPool() {
    vkResetDescriptorPool(device_.getDevice(), descriptorPool_, 0);
  }

  // *************** Descriptor Writer *********************

  DescriptorWriter::DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool)
      : setLayout_{setLayout}, pool_{pool} {}

  DescriptorWriter &DescriptorWriter::writeBuffer(uint32_t binding,
                                                  VkDescriptorBufferInfo *bufferInfo) {
    assert(setLayout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout_.bindings_[binding];

    assert(bindingDescription.descriptorCount == 1
           && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    writes_.push_back(write);
    return *this;
  }

  DescriptorWriter &DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo,
                                                 uint32_t count) {
    assert(setLayout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout_.bindings_[binding];

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = count;

    writes_.push_back(write);
    return *this;
  }

  bool DescriptorWriter::build(VkDescriptorSet &set) {
    bool success = pool_.allocateDescriptor(setLayout_.getDescriptorSetLayout(), set);
    if (!success) {
      return false;
    }
    overwrite(set);
    return true;
  }

  void DescriptorWriter::overwrite(VkDescriptorSet &set) {
    for (auto &write : writes_) {
      write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool_.device_.getDevice(), writes_.size(), writes_.data(), 0, nullptr);
  }

}  // namespace ve