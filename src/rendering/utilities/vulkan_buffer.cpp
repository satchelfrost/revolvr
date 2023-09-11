#include <rendering/utilities/vulkan_buffer.h>
#include "rendering/utilities/vulkan_results.h"

namespace rvr {
VulkanBuffer::VulkanBuffer(const std::shared_ptr<RenderingContext> &context, size_t sizeOfElement,
                           size_t count, VkBufferUsageFlags usage, MemoryType memType,
                           VkDeviceSize minOffsetAlignment) : rendering_context_(context), count_(count),
                           device_(context->GetDevice()), buffer_(nullptr),
                           memory_(nullptr), sizeOfElement_(sizeOfElement) {
    auto memoryPropertyFlags = GetMemoryPropertyFlags(memType);
    alignmentSize_ = GetAlignment((VkDeviceSize)sizeOfElement, minOffsetAlignment);
    sizeOfBuffer_ = alignmentSize_ * count_;

    context->CreateBuffer(sizeOfBuffer_, usage, memoryPropertyFlags, &buffer_, &memory_);

    bufferInfo_.offset = 0;
    bufferInfo_.buffer = buffer_;
    bufferInfo_.range = sizeOfBuffer_;
}

VkDeviceSize VulkanBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

void VulkanBuffer::CopyFrom(const std::shared_ptr<VulkanBuffer>& src, size_t size, size_t srcOffset, size_t dstOffset) {
    rendering_context_->CopyBuffer(src->buffer_, buffer_, size, srcOffset, dstOffset);
}

VkBuffer VulkanBuffer::GetBuffer() const {
    return buffer_;
}

VulkanBuffer::~VulkanBuffer() {
    Unmap();
    vkDestroyBuffer(device_, buffer_, nullptr);
    vkFreeMemory(device_, memory_, nullptr);
}

uint32_t VulkanBuffer::GetCount() const {
    return count_;
}

size_t VulkanBuffer::GetSizeOfBuffer() const {
    return sizeOfBuffer_;
}

VkDescriptorBufferInfo VulkanBuffer::GetBufferInfo() {
    return bufferInfo_;
}

void VulkanBuffer::Map(VkDeviceSize size, VkDeviceSize offset) {
    if (!(buffer_ && memory_))
        THROW("Called map on buffer before create");
    VkResult result = vkMapMemory(device_, memory_, offset, size, 0, &mappedData_);
    CHECK_VKCMD(result);
}

void VulkanBuffer::Unmap() {
    if (mappedData_) {
        vkUnmapMemory(device_, memory_);
        mappedData_ = nullptr;
    }
}

void VulkanBuffer::WriteToBuffer(const void *data, VkDeviceSize size, VkDeviceSize offset) {
    if (!mappedData_)
        THROW("Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        memcpy(mappedData_, data, sizeOfBuffer_);
    }
    else {
        char *memOffset = (char *)mappedData_;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

VkResult VulkanBuffer::Flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(device_, 1, &mappedRange);
}

VkResult VulkanBuffer::Invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(device_, 1, &mappedRange);
}

VkDescriptorBufferInfo VulkanBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo{
            buffer_,
            offset,
            size,
    };
}

void VulkanBuffer::WriteToIndex(void *data, int index) {
    WriteToBuffer(data, sizeOfElement_, index * alignmentSize_);
}

VkResult VulkanBuffer::FlushIndex(int index) {
    return Flush(alignmentSize_, index * alignmentSize_);
}

VkDescriptorBufferInfo VulkanBuffer::DescriptorInfoForIndex(int index) {
    return descriptorInfo(alignmentSize_, index * alignmentSize_);
}

VkResult VulkanBuffer::InvalidateIndex(int index) {
    return Invalidate(alignmentSize_, index * alignmentSize_);
}
}