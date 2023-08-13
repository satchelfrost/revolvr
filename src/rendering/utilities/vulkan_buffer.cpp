#include <rendering/utilities/vulkan_buffer.h>
#include "rendering/utilities/vulkan_results.h"

namespace rvr {
VulkanBuffer::VulkanBuffer(const std::shared_ptr<RenderingContext> &context, size_t length,
                                VkBufferUsageFlags usage) :
rendering_context_(context), sizeInBytes_(length), device_(context->GetDevice()) {
    context->CreateBuffer(length, usage, &buffer_, &memory_);
}

void VulkanBuffer::Update(const void *data) {
    void* mappedData = nullptr;
    VkResult result = vkMapMemory(device_, memory_, 0, sizeInBytes_, 0,
                                  &mappedData);
    CHECK_VKCMD(result);
    memcpy(mappedData, data, sizeInBytes_);
    vkUnmapMemory(device_, memory_);
}

void VulkanBuffer::CopyFrom(std::shared_ptr<VulkanBuffer> src, size_t size, size_t srcOffset, size_t dstOffset) {
    rendering_context_->CopyBuffer(src->buffer_, buffer_, size, srcOffset, dstOffset);
}

VkBuffer VulkanBuffer::GetBuffer() const {
    return buffer_;
}

size_t VulkanBuffer::GetSizeInBytes() const {
    return sizeInBytes_;
}

VulkanBuffer::~VulkanBuffer() {
    vkDestroyBuffer(device_, buffer_, nullptr);
    vkFreeMemory(device_, memory_, nullptr);
}
}