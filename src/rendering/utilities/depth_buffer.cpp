/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/depth_buffer.h>
#include <rendering/utilities/command_buffer.h>
#include <rendering/utilities/vulkan_results.h>
#include <rendering/utilities/vulkan_utils.h>

namespace rvr {
DepthBuffer::~DepthBuffer() {
    if (device_ != nullptr) {
        if (depthImage_ != VK_NULL_HANDLE) {
            vkDestroyImage(device_, depthImage_, nullptr);
        }
        if (depthMemory_ != VK_NULL_HANDLE) {
            vkFreeMemory(device_, depthMemory_, nullptr);
        }
    }
    depthImage_ = VK_NULL_HANDLE;
    depthMemory_ = VK_NULL_HANDLE;
    device_ = nullptr;
}

DepthBuffer::DepthBuffer(DepthBuffer &&other) noexcept: DepthBuffer() {
    using std::swap;

    swap(depthImage_, other.depthImage_);
    swap(depthMemory_, other.depthMemory_);
    swap(device_, other.device_);
}

DepthBuffer &DepthBuffer::operator=(DepthBuffer &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    // clean up self
    this->~DepthBuffer();
    using std::swap;

    swap(depthImage_, other.depthImage_);
    swap(depthMemory_, other.depthMemory_);
    swap(device_, other.device_);
    return *this;
}

DepthBuffer::DepthBuffer(const std::shared_ptr<RenderingContext> &context, VkExtent2D extent,
                         VkSampleCountFlagBits samples, VkCommandBuffer cmdBuffer, VkImageLayout newLayout):
device_(context->GetDevice()) {
    // Create a D32 depth buffer
    VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = context->GetDepthFormat();
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = samples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    CHECK_VKCMD(vkCreateImage(context->GetDevice(), &imageInfo, nullptr, &depthImage_));

    VkMemoryRequirements memRequirements{};
    vkGetImageMemoryRequirements(context->GetDevice(), depthImage_, &memRequirements);
    uint32_t memoryIdx = FindMemoryType(context->GetPhysicalDevice(),memRequirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkMemoryAllocateInfo allocInfo = CreateMemAllocInfo(memRequirements.size, memoryIdx);

    VkResult result = vkAllocateMemory(context->GetDevice(), &allocInfo, nullptr, &depthMemory_);
    CHECK_VKCMD(result);

    CHECK_VKCMD(vkBindImageMemory(context->GetDevice(), depthImage_, depthMemory_, 0));
}

void DepthBuffer::TransitionLayout(VkCommandBuffer cmdBuffer, VkImageLayout newLayout) {
    if (newLayout == imageLayout_)
        return;

    VkImageMemoryBarrier depthBarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    depthBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    depthBarrier.oldLayout = imageLayout_;
    depthBarrier.newLayout = newLayout;
    depthBarrier.image = depthImage_;
    depthBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                         VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &depthBarrier);

    imageLayout_ = newLayout;
}

}