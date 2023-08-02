#include "buffer.hpp"

// std
#include <cassert>
#include <cstring>

namespace ve {

  /**
   * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
   *
   * @param instanceSize The size of an instance
   * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
   * minUniformBufferOffsetAlignment)
   *
   * @return VkResult of the buffer mapping call
   */
  VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
      return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
  }

  Buffer::Buffer(Device &device, VkDeviceSize instanceSize, uint32_t instanceCount,
                 VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
                 VkDeviceSize minOffsetAlignment)
      : device_{device},
        instanceCount_{instanceCount},
        instanceSize_{instanceSize},
        usageFlags_{usageFlags},
        memoryPropertyFlags_{memoryPropertyFlags} {
    alignmentSize_ = getAlignment(instanceSize, minOffsetAlignment);
    bufferSize_ = alignmentSize_ * instanceCount;
    device.createBuffer(bufferSize_, usageFlags, memoryPropertyFlags, buffer_, memory_);
  }

  Buffer::~Buffer() {
    unmap();
    vkDestroyBuffer(device_.getDevice(), buffer_, nullptr);
    vkFreeMemory(device_.getDevice(), memory_, nullptr);
  }

  /**
   * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
   *
   * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
   * buffer range.
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkResult of the buffer mapping call
   */
  VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
    assert(buffer_ && memory_ && "Called map on buffer before create");
    return vkMapMemory(device_.getDevice(), memory_, offset, size, 0, &mapped_);
  }

  /**
   * Unmap a mapped memory range
   *
   * @note Does not return a result as vkUnmapMemory can't fail
   */
  void Buffer::unmap() {
    if (mapped_ != nullptr) {
      vkUnmapMemory(device_.getDevice(), memory_);
      mapped_ = nullptr;
    }
  }

  /**
   * Copies the specified data to the mapped buffer. Default value writes whole buffer range
   *
   * @param data Pointer to the data to copy
   * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete
   * buffer range.
   * @param offset (Optional) Byte offset from beginning of mapped region
   *
   */
  void Buffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
    assert(mapped_ && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
      memcpy(mapped_, data, bufferSize_);
    } else {
      char *memOffset = (char *)mapped_;
      memOffset += offset;
      memcpy(memOffset, data, size);
    }
  }

  /**
   * Flush a memory range of the buffer to make it visible to the device
   *
   * @note Only required for non-coherent memory
   *
   * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
   * complete buffer range.
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkResult of the flush call
   */
  VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(device_.getDevice(), 1, &mappedRange);
  }

  /**
   * Invalidate a memory range of the buffer to make it visible to the host
   *
   * @note Only required for non-coherent memory
   *
   * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
   * the complete buffer range.
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkResult of the invalidate call
   */
  VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(device_.getDevice(), 1, &mappedRange);
  }

  /**
   * Create a buffer info descriptor
   *
   * @param size (Optional) Size of the memory range of the descriptor
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkDescriptorBufferInfo of specified offset and range
   */
  VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo{
        buffer_,
        offset,
        size,
    };
  }

  /**
   * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
   *
   * @param data Pointer to the data to copy
   * @param index Used in offset calculation
   *
   */
  void Buffer::writeToIndex(void *data, int index) {
    writeToBuffer(data, instanceSize_, index * alignmentSize_);
  }

  /**
   *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
   *
   * @param index Used in offset calculation
   *
   */
  VkResult Buffer::flushIndex(int index) { return flush(alignmentSize_, index * alignmentSize_); }

  /**
   * Create a buffer info descriptor
   *
   * @param index Specifies the region given by index * alignmentSize
   *
   * @return VkDescriptorBufferInfo for instance at index
   */
  VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) {
    return descriptorInfo(alignmentSize_, index * alignmentSize_);
  }

  VkResult Buffer::invalidateIndex(int index) {
    return invalidate(alignmentSize_, index * alignmentSize_);
  }

}  // namespace ve
