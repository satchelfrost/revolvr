/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/rendering_context.h>

namespace rvr {
class RenderTarget {
private:
    VkDevice device_{VK_NULL_HANDLE};
    VkImage colorImage_{VK_NULL_HANDLE};
    VkImage depthImage_{VK_NULL_HANDLE};
    VkImageView colorView_{VK_NULL_HANDLE};
    VkImageView depthView_{VK_NULL_HANDLE};
    VkFramebuffer framebuffer_{VK_NULL_HANDLE};
public:
    void Create(const std::shared_ptr<RenderingContext>& context, VkImage colorImage, VkImage depthImage, VkExtent2D size);
    RenderTarget() = default;
    RenderTarget(RenderTarget &&other) noexcept;
    RenderTarget &operator=(RenderTarget &&other) noexcept;
    RenderTarget(const RenderTarget &) = delete;
    RenderTarget &operator=(const RenderTarget &) = delete;
    VkFramebuffer GetFramebuffer();
    ~RenderTarget();
};
}