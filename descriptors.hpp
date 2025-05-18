#pragma once

#include "device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace ve {

  class DescriptorSetLayout {
  public:
    class Builder {
    public:
      Builder(Device &device) : device_{device} {}

      Builder &addBinding(uint32_t binding, VkDescriptorType descriptorType,
                          VkShaderStageFlags stageFlags, uint32_t count = 1);
      std::unique_ptr<DescriptorSetLayout> build() const;

    private:
      Device &device_;
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;
    };

    DescriptorSetLayout(Device &device,
                        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout_; }

  private:
    Device &device_;
    VkDescriptorSetLayout descriptorSetLayout_;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;

    friend class DescriptorWriter;
  };

  class DescriptorSetLayoutPush {
  public:
    class Builder {
    public:
      Builder(Device &device) : device_{device} {}

      Builder &addBinding(uint32_t binding, VkDescriptorType descriptorType,
                          VkShaderStageFlags stageFlags, uint32_t count = 1);
      std::unique_ptr<DescriptorSetLayoutPush> build() const;

    private:
      Device &device_;
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;
    };

    DescriptorSetLayoutPush(Device &device,
                            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~DescriptorSetLayoutPush();
    DescriptorSetLayoutPush(const DescriptorSetLayoutPush &) = delete;
    DescriptorSetLayoutPush &operator=(const DescriptorSetLayoutPush &) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayoutPush_; }

  private:
    Device &device_;
    VkDescriptorSetLayout descriptorSetLayoutPush_;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;

    friend class DescriptorWriter;
  };

  class DescriptorPool {
  public:
    class Builder {
    public:
      Builder(Device &device) : device_{device} {}

      Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
      Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
      Builder &setMaxSets(uint32_t count);
      std::unique_ptr<DescriptorPool> build() const;

    private:
      Device &device_;
      std::vector<VkDescriptorPoolSize> poolSizes_;
      uint32_t maxSets_ = 1000;
      VkDescriptorPoolCreateFlags poolFlags_ = 0;
    };

    DescriptorPool(Device &device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags,
                   const std::vector<VkDescriptorPoolSize> &poolSizes);
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool &) = delete;
    DescriptorPool &operator=(const DescriptorPool &) = delete;

    bool allocateDescriptor(VkDescriptorSetLayout descriptorSetLayout,
                            VkDescriptorSet &descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

    void resetPool();

  private:
    Device &device_;
    VkDescriptorPool descriptorPool_;

    friend class DescriptorWriter;
  };

  class DescriptorWriter {
  public:
    DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool);

    DescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    DescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo,
                                 uint32_t count = 1);

    bool build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet &set);

  private:
    DescriptorSetLayout &setLayout_;
    DescriptorPool &pool_;
    std::vector<VkWriteDescriptorSet> writes_;
  };

}  // namespace ve