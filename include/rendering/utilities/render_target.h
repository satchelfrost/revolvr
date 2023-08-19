/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/rendering_context.h>
#include <rendering/utilities/vulkan_image.h>
#include <rendering/utilities/vulkan_view.h>

namespace rvr {
class RenderTarget {
private:
    // OpenXR creates a color image for us, so no need to create one here
    VulkanView* colorView_;
    VulkanImage* depthImage_;
    VulkanView* depthView_;
    VkFramebuffer framebuffer_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};
public:
    RenderTarget(const std::shared_ptr<RenderingContext>& context, VkImage colorImage, VkExtent2D extent);
    VkFramebuffer GetFramebuffer();
    ~RenderTarget();
};
}