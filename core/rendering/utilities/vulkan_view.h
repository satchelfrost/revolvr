/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <rendering/utilities/rendering_context.h>

namespace rvr {
class VulkanView {
public:
    enum ViewType {
        Depth,
        Color,
        Sample
    };

private:
    VkImageView imageView_;
    VkDevice device_;

public:
    VulkanView(const std::shared_ptr<RenderingContext>& context, ViewType viewType, VkImage image);
    VkImageView GetImageView();
    ~VulkanView();
};
}
