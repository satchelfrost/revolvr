/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "buffer.h"
#include "cmd_buffer.h"
#include "vk_results.h"

namespace rvr {
Buffer::Buffer(const std::shared_ptr<RenderingContext> &context, size_t sizeOfElement,
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

VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

void Buffer::CopyFrom(const std::shared_ptr<Buffer>& src, size_t size, size_t srcOffset, size_t dstOffset) {
    CmdBuffer cmd = CmdBuffer(rendering_context_->GetDevice(), rendering_context_->GetGraphicsPool());
    cmd.Begin();
    rendering_context_->CopyBuffer(cmd.GetBuffer(), src->buffer_, buffer_, size, srcOffset,
                                   dstOffset);
    cmd.End();
    cmd.Exec(rendering_context_->GetGraphicsQueue());
}

VkBuffer Buffer::GetBuffer() const {
    return buffer_;
}

Buffer::~Buffer() {
    Unmap();
    vkDestroyBuffer(device_, buffer_, nullptr);
    vkFreeMemory(device_, memory_, nullptr);
}

uint32_t Buffer::GetCount() const {
    return count_;
}

size_t Buffer::GetSizeOfBuffer() const {
    return sizeOfBuffer_;
}

void Buffer::Map(VkDeviceSize size, VkDeviceSize offset) {
    if (!(buffer_ && memory_))
        THROW("Called map on buffer before create");
    VkResult result = vkMapMemory(device_, memory_, offset, size, 0, &mappedData_);
    CHECK_VKCMD(result);
}

void Buffer::Unmap() {
    if (mappedData_) {
        vkUnmapMemory(device_, memory_);
        mappedData_ = nullptr;
    }
}

void Buffer::WriteToBuffer(const void *data, VkDeviceSize size, VkDeviceSize offset) {
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

VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(device_, 1, &mappedRange);
}

VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(device_, 1, &mappedRange);
}

VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo{
            buffer_,
            offset,
            size,
    };
}

void Buffer::WriteToIndex(void *data, int index) {
    WriteToBuffer(data, sizeOfElement_, index * alignmentSize_);
}

VkResult Buffer::FlushIndex(int index) {
    return Flush(alignmentSize_, index * alignmentSize_);
}

VkDescriptorBufferInfo Buffer::DescriptorInfoForIndex(int index) {
    return DescriptorInfo(alignmentSize_, index * alignmentSize_);
}

VkResult Buffer::InvalidateIndex(int index) {
    return Invalidate(alignmentSize_, index * alignmentSize_);
}
}
