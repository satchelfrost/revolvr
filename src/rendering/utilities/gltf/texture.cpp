/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/gltf/texture.h>
#include <rendering/utilities/command_buffer.h>
#include <global_context.h>
#include "rendering/utilities/vulkan_results.h"

namespace rvr {
void VulkanTexture::UpdateDescriptor() {
    descriptor.sampler = sampler;
    descriptor.imageView = view_->GetImageView();
    descriptor.imageLayout = imageLayout_;
}

void VulkanTexture::Destroy() {
    if (sampler)
        vkDestroySampler(device_, sampler, nullptr);
    vkFreeMemory(device_, deviceMemory, nullptr);
}

void VulkanTexture::FromBuffer(void* buffer, VkDeviceSize bufferSize, uint32_t texWidth,
uint32_t texHeight, const std::shared_ptr<RenderingContext>& renderingContext, VkFilter filter,
VkImageLayout imageLayout) {
    assert(buffer);

    device_ = renderingContext->GetDevice();
    width = texWidth;
    height = texHeight;
    mipLevels = 1;

    VulkanBuffer stagingBuffer = VulkanBuffer(renderingContext, 1, bufferSize,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MemoryType::HostVisible);
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer(buffer);
    stagingBuffer.Unmap();

    VkBufferImageCopy bufferCopyRegion = {};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.mipLevel = 0;
    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = width;
    bufferCopyRegion.imageExtent.height = height;
    bufferCopyRegion.imageExtent.depth = 1;
    bufferCopyRegion.bufferOffset = 0;

    VkExtent2D extent = {width, height};
    image_ = std::make_unique<VulkanImage>(renderingContext, VulkanImage::Sample, extent);

    CommandBuffer cmd = CommandBuffer(renderingContext->GetDevice(), renderingContext->GetGraphicsPool());
    cmd.Begin();
    renderingContext->CreateTransitionLayout(cmd.GetBuffer(), image_->GetImage(),
                                             VK_IMAGE_LAYOUT_UNDEFINED,
                                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    // Copy mip levels from staging buffer
    vkCmdCopyBufferToImage(cmd.GetBuffer(), stagingBuffer.GetBuffer(),
                           image_->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &bufferCopyRegion);

    imageLayout_ = imageLayout;
    renderingContext->CreateTransitionLayout(cmd.GetBuffer(), image_->GetImage(),
                                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageLayout);
    cmd.End();
    cmd.Exec(renderingContext->GetGraphicsQueue());

    // TODO: may want to create a sampler class
    // Create sampler
    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = filter;
    samplerCreateInfo.minFilter = filter;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = 0.0f;
    samplerCreateInfo.maxAnisotropy = 1.0f;
    VkResult result = vkCreateSampler(device_, &samplerCreateInfo, nullptr, &sampler);
    CHECK_VKCMD(result);

    view_ = std::make_unique<VulkanView>(renderingContext, VulkanView::Sample,
                                         image_->GetImage());

    // Update descriptor image info member that can be used for setting up descriptor sets
    UpdateDescriptor();
}
}