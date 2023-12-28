/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include "rendering_context.h"
#include "vk_utils.h"

namespace rvr {
class Buffer {
private:
    std::shared_ptr<RenderingContext> rendering_context_;
    size_t sizeOfBuffer_;
    size_t sizeOfElement_;
    VkBuffer buffer_;
    VkDeviceMemory memory_;
    VkDevice device_;
    uint32_t count_;
    void* mappedData_ = nullptr;
    VkDescriptorBufferInfo bufferInfo_;
    VkDeviceSize alignmentSize_;
    static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

public:
    // For vertex/index buffers minOffsetAlignment can remain default. It matters for uniform buffers.
    Buffer(const std::shared_ptr<RenderingContext> &context, size_t sizeOfElement, size_t count,
           VkBufferUsageFlags usage, MemoryType memType, VkDeviceSize minOffsetAlignment = 1);

    void Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void Unmap();

    void WriteToBuffer(const void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void WriteToIndex(void* data, int index);
    VkResult FlushIndex(int index);
    VkDescriptorBufferInfo DescriptorInfoForIndex(int index);
    VkResult InvalidateIndex(int index);

    void CopyFrom(const std::shared_ptr<Buffer>& src, size_t size, size_t srcOffset, size_t dstOffset);
    VkBuffer GetBuffer() const;
    uint32_t GetCount() const;
    size_t GetSizeOfBuffer() const;
    ~Buffer();
};
}