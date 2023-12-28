/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "texture.h"
#include "cmd_buffer.h"
#include "vk_results.h"
#include <global_context.h>

namespace rvr {
void Texture::UpdateDescriptor() {
    descriptor.sampler = sampler->GetSampler();
    descriptor.imageView = view->GetImageView();
    descriptor.imageLayout = imageLayout;
}

void Texture::FromBuffer(void* buffer, VkDeviceSize bufferSize, uint32_t texWidth,
                         uint32_t texHeight, const std::shared_ptr<RenderingContext>& renderingContext,
                         VkFilter filter, VkImageLayout imgLayout) {
    assert(buffer);

    device = renderingContext->GetDevice();
    width = texWidth;
    height = texHeight;
    mipLevels = 1;

    Buffer stagingBuffer = Buffer(renderingContext, 1, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  MemoryType::HostVisible);
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
    image = std::make_unique<Image>(renderingContext, Image::Sample, extent);

    CmdBuffer cmd = CmdBuffer(renderingContext->GetDevice(), renderingContext->GetGraphicsPool());
    cmd.Begin();
    renderingContext->CreateTransitionLayout(cmd.GetBuffer(), image->GetImage(),
                                             VK_IMAGE_LAYOUT_UNDEFINED,
                                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    // Copy mip levels from staging buffer
    vkCmdCopyBufferToImage(cmd.GetBuffer(), stagingBuffer.GetBuffer(),
                           image->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &bufferCopyRegion);

    imageLayout = imgLayout;
    renderingContext->CreateTransitionLayout(cmd.GetBuffer(), image->GetImage(),
                                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imgLayout);
    cmd.End();
    cmd.Exec(renderingContext->GetGraphicsQueue());

    sampler = std::make_unique<Sampler>(device, filter);
    view = std::make_unique<View>(renderingContext, View::Sample, image->GetImage());

    // Update descriptor image info member that can be used for setting up descriptor sets
    UpdateDescriptor();
}
}
