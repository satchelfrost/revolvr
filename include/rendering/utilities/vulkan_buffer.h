#pragma once

#include <pch.h>
#include <rendering/utilities/rendering_context.h>
#include "vulkan_utils.h"

namespace rvr {
class VulkanBuffer {
private:
    std::shared_ptr<RenderingContext> rendering_context_;
    size_t sizeInBytes_;
    VkBuffer buffer_{};
    VkDeviceMemory memory_{};
    VkDevice device_;
    uint32_t count_;

public:
    // TODO: implement a constructor where you don't own the memory
    VulkanBuffer(const std::shared_ptr<RenderingContext> &context, size_t sizeOfElement, size_t count,
                 VkBufferUsageFlags usage);
    void Update(const void *data);
    void CopyFrom(const std::shared_ptr<VulkanBuffer>& src, size_t size, size_t srcOffset, size_t dstOffset);
    VkBuffer GetBuffer() const;
    uint32_t GetCount() const;
    size_t GetSizeInBytes() const;
    ~VulkanBuffer();
};
}