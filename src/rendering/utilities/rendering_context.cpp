#include <rendering/utilities/rendering_context.h>

#include <rendering/utilities/vulkan_utils.h>
#include <rendering/utilities/rendering_context.h>
#include "rendering/utilities/vulkan_results.h"
#include "rendering/utilities/command_buffer.h"

namespace rvr {
RenderingContext::RenderingContext(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue graphicsQueue,
                                   VkFormat colorFormat, VkCommandPool graphicsPool) :
physicalDevice_(physicalDevice), device_(device), graphicsQueue_(graphicsQueue), colorFormat_(colorFormat),
graphicsPool_(graphicsPool) {
    renderPass_.Create(device, colorFormat, depthFormat_);
}

//uint32_t RenderingContext::GetGraphicsQueueFamilyIndex() {
//    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_);
//    indices.graphicsFamily.value();
//}

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
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
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

VkCommandPool RenderingContext::GetGraphicsPool() {
    return graphicsPool_;
}

void RenderingContext::CreateImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags usage, VkImage *image,
                                   VkDeviceMemory *imageMemory) {
    VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(device_, &imageInfo, nullptr, image);
    CHECK_VKCMD(result);
    AllocateImageMemory(*image, imageMemory);
    result = vkBindImageMemory(device_, *image, *imageMemory, 0);
    CHECK_VKCMD(result);
}

void RenderingContext::AllocateImageMemory(VkImage image, VkDeviceMemory *memory) {
    VkMemoryRequirements memRequirements{};
    vkGetImageMemoryRequirements(device_, image, &memRequirements);
    uint32_t memoryIdx = FindMemoryType(physicalDevice_,memRequirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkMemoryAllocateInfo allocInfo = CreateMemAllocInfo(memRequirements.size, memoryIdx);

    VkResult result = vkAllocateMemory(device_, &allocInfo, nullptr, memory);
    CHECK_VKCMD(result);
}

void RenderingContext::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspectMask,
                                       VkImageView *imageView) {
    VkImageViewCreateInfo colorViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    colorViewInfo.image = image;
    colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorViewInfo.format = format;
    colorViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    colorViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    colorViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    colorViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    colorViewInfo.subresourceRange.aspectMask = aspectMask;
    colorViewInfo.subresourceRange.baseMipLevel = 0;
    colorViewInfo.subresourceRange.levelCount = 1;
    colorViewInfo.subresourceRange.baseArrayLayer = 0;
    colorViewInfo.subresourceRange.layerCount = 1;
    VkResult result = vkCreateImageView(device_, &colorViewInfo, nullptr, imageView);
    CHECK_VKCMD(result);
}

void RenderingContext::CreateTransitionLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage{};
    VkPipelineStageFlags dstStage{};
    PopulateTransitionLayoutInfo(barrier, srcStage, dstStage, oldLayout, newLayout);

    CommandBuffer commandBuffer(device_, graphicsPool_);
    commandBuffer.Begin();
    vkCmdPipelineBarrier(commandBuffer.GetBuffer(), srcStage, dstStage,
                         0, 0, nullptr,0,
                         nullptr, 1, &barrier);
    commandBuffer.End();
    commandBuffer.Exec(graphicsQueue_);
}

}
