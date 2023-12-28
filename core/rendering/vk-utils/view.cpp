/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "view.h"

namespace rvr {
View::View(const std::shared_ptr<RenderingContext>& context, ViewType viewType, VkImage image) :
device_(context->GetDevice()) {
    VkFormat format;
    VkImageAspectFlagBits aspectMask;
    switch (viewType) {
        case Depth:
            format = context->GetDepthFormat();
            aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
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


View::~View() {
    vkDestroyImageView(device_, imageView_, nullptr);
}

VkImageView View::GetImageView() {
    return imageView_;
}
}
