#include <rendering/utilities/rendering_context.h>

#include <rendering/utilities/vulkan_utils.h>
#include <rendering/utilities/rendering_context.h>
#include "rendering/utilities/vulkan_results.h"

namespace rvr {
RenderingContext::RenderingContext(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue,
                                   VkFormat colorFormat) :
physicalDevice_(physicalDevice), device_(device), graphicsQueue_(graphicsQueue), colorFormat_(colorFormat) {
    renderPass_.Create(device, colorFormat, depthFormat_);
}

uint32_t RenderingContext::GetGraphicsQueueFamilyIndex() {
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_);
    indices.graphicsFamily.value();
}

VkDevice RenderingContext::GetDevice() {
    return device_;
}

VkPhysicalDevice RenderingContext::GetPhysicalDevice() {
    return physicalDevice_;
}

VkQueue RenderingContext::GetGraphicsQueue() {
    return graphicsQueue_;
}

VkFormat RenderingContext::GetColorFormat() {
    return colorFormat_;
}
VkFormat RenderingContext::GetDepthFormat() {
    return depthFormat_;
}

VkRenderPass RenderingContext::GetRenderPass() const {
    return renderPass_.pass;
}

void RenderingContext::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer *buffer,
                                    VkDeviceMemory *memory) {
    VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufInfo.usage = usage;
    bufInfo.size = size;
    VkResult result = vkCreateBuffer(device_, &bufInfo, nullptr, buffer);
    CHECK_VKCMD(result);
    AllocateBufferMemory(*buffer, memory);
    result = vkBindBufferMemory(device_, *buffer, *memory, 0);
    CHECK_VKCMD(result);
}

void RenderingContext::AllocateBufferMemory(VkBuffer buffer, VkDeviceMemory *memory) {
    VkMemoryRequirements memReq = {};
    vkGetBufferMemoryRequirements(device_, buffer, &memReq);
    uint32_t memIdx = FindMemoryType(physicalDevice_, memReq.memoryTypeBits,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = memIdx;
    VkResult result = vkAllocateMemory(device_, &allocInfo, nullptr, memory);
    CHECK_VKCMD(result);
}

void RenderingContext::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset,
                                  VkDeviceSize dstOffset) {
    // TODO
}
}
