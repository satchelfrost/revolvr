/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/vulkan_view.h>

namespace rvr {
VulkanView::VulkanView(const std::shared_ptr<RenderingContext>& context, ViewType viewType, VkImage image) :
device_(context->GetDevice()) {
    VkFormat format;
    VkImageAspectFlags aspectMask;
    switch (viewType) {
        case Depth:
            format = context->GetDepthFormat();
            aspectMask = (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
            break;
        case Color:
            format = context->GetColorFormat();
            aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            break;
        case Sample:
            format = VK_FORMAT_R8G8B8A8_UNORM;
            aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            break;
        default:
            THROW("Image type undefined");
    }

    context->CreateImageView(image, format, aspectMask, &imageView_);
}


VulkanView::~VulkanView() {
    vkDestroyImageView(device_, imageView_, nullptr);
}

VkImageView VulkanView::GetImageView() {
    return imageView_;
}
}
