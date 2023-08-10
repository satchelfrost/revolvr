#pragma once

#include <pch.h>
#include <rendering/utilities/rendering_context.h>

namespace rvr {
class VulkanBuffer {
private:
    std::shared_ptr<RenderingContext> rendering_context_;
    size_t sizeInBytes_;
    VkBuffer buffer_{};
    VkDeviceMemory memory_{};
    VkDevice device_;

public:
    VulkanBuffer(const std::shared_ptr<RenderingContext> &context, size_t length, VkBufferUsageFlags usage);
    void Update(const void *data);
    void CopyFrom(std::shared_ptr<VulkanBuffer> src, size_t size, size_t srcOffset, size_t dstOffset);
    VkBuffer GetBuffer() const;
    size_t GetSizeInBytes() const;
    ~VulkanBuffer();
};
}