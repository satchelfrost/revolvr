/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/vulkan_image.h>

namespace rvr {
VulkanImage::VulkanImage(const std::shared_ptr<RenderingContext>& context, VulkanImage::ImageType imageType,
                         VkExtent2D extent) :
device_(context->GetDevice()) {
    VkFormat format;
    VkImageUsageFlags usage;
    switch (imageType) {
        case Depth:
            format = context->GetDepthFormat();
            usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            break;
        case Color:
            format = context->GetColorFormat();
            usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            break;
        case Sample:
            format = VK_FORMAT_R8G8B8A8_UNORM;
            usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            break;
        default:
            THROW("Image type undefined");
    }

    context->CreateImage(extent, format, usage, &image_, &imageMemory_);
}


VulkanImage::~VulkanImage() {
    vkDestroyImage(device_, image_, nullptr);
    vkFreeMemory(device_, imageMemory_, nullptr);
}

VkImage VulkanImage::GetImage() {
    return image_;
}
}
