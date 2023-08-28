#include <rendering/utilities/vulkan_buffer.h>
#include "rendering/utilities/vulkan_results.h"

namespace rvr {
VulkanBuffer::VulkanBuffer(const std::shared_ptr<RenderingContext> &context, size_t sizeOfElement,
                           size_t count, VkBufferUsageFlags usage, MemoryType memType) :
                           rendering_context_(context), count_(count), sizeInBytes_(count * sizeOfElement),
                           device_(context->GetDevice()), buffer_(nullptr), memory_(nullptr) {
    auto memoryPropertyFlags = GetMemoryPropertyFlags(memType);
    context->CreateBuffer(sizeInBytes_, usage, memoryPropertyFlags, &buffer_, &memory_);
}

void VulkanBuffer::Update(const void *data) {
    void* mappedData = nullptr;
    VkResult result = vkMapMemory(device_, memory_, 0, sizeInBytes_, 0,
                                  &mappedData);
    CHECK_VKCMD(result);
    memcpy(mappedData, data, sizeInBytes_);
    vkUnmapMemory(device_, memory_);
}

void VulkanBuffer::CopyFrom(const std::shared_ptr<VulkanBuffer>& src, size_t size, size_t srcOffset, size_t dstOffset) {
    rendering_context_->CopyBuffer(src->buffer_, buffer_, size, srcOffset, dstOffset);
}

VkBuffer VulkanBuffer::GetBuffer() const {
    return buffer_;
}

VulkanBuffer::~VulkanBuffer() {
    vkDestroyBuffer(device_, buffer_, nullptr);
    vkFreeMemory(device_, memory_, nullptr);
}

uint32_t VulkanBuffer::GetCount() const {
    return count_;
}

size_t VulkanBuffer::GetSizeInBytes() const {
    return sizeInBytes_;
}
}