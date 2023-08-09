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
    if (m_vkDevice != nullptr) {
        if (depthImage != VK_NULL_HANDLE) {
            vkDestroyImage(m_vkDevice, depthImage, nullptr);
        }
        if (depthMemory_ != VK_NULL_HANDLE) {
            vkFreeMemory(m_vkDevice, depthMemory_, nullptr);
        }
    }
    depthImage = VK_NULL_HANDLE;
    depthMemory_ = VK_NULL_HANDLE;
    m_vkDevice = nullptr;
}

DepthBuffer::DepthBuffer(DepthBuffer &&other) noexcept: DepthBuffer() {
    using std::swap;

    swap(depthImage, other.depthImage);
    swap(depthMemory_, other.depthMemory_);
    swap(m_vkDevice, other.m_vkDevice);
}

DepthBuffer &DepthBuffer::operator=(DepthBuffer &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    // clean up self
    this->~DepthBuffer();
    using std::swap;

    swap(depthImage, other.depthImage);
    swap(depthMemory_, other.depthMemory_);
    swap(m_vkDevice, other.m_vkDevice);
    return *this;
}

void DepthBuffer::Create(const std::shared_ptr<RenderingContext>& context, const std::shared_ptr<SwapchainImageContext>& swapchainImageContext) {

    m_vkDevice = context->GetDevice();

    // Create a D32 depth buffer
    VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = swapchainImageContext.GetSwapchainExtent().width;
    imageInfo.extent.height = swapchainImageContext.GetSwapchainExtent().height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = context->GetDepthFormat();
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = swapchainImageContext.GetSampleFlagBits();
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    CHECK_VKCMD(vkCreateImage(context->GetDevice(), &imageInfo, nullptr, &depthImage));

    VkMemoryRequirements memRequirements{};
    vkGetImageMemoryRequirements(context->GetDevice(), depthImage, &memRequirements);
    uint32_t memoryIdx = FindMemoryType(context->GetPhysicalDevice(),memRequirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkMemoryAllocateInfo allocInfo = CreateMemAllocInfo(memRequirements.size, memoryIdx);

    VkResult result = vkAllocateMemory(context->GetDevice(), &allocInfo, nullptr, &depthMemory_);
    CHECK_VKCMD(result);

    CHECK_VKCMD(vkBindImageMemory(context->GetDevice(), depthImage, depthMemory_, 0));
}

void DepthBuffer::TransitionLayout(CmdBuffer *cmdBuffer, VkImageLayout newLayout) {
    if (newLayout == m_vkLayout) {
        return;
    }

    VkImageMemoryBarrier depthBarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    depthBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    depthBarrier.oldLayout = m_vkLayout;
    depthBarrier.newLayout = newLayout;
    depthBarrier.image = depthImage;
    depthBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
    vkCmdPipelineBarrier(cmdBuffer->buf, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                         VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &depthBarrier);

    m_vkLayout = newLayout;
}
}