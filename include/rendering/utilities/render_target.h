/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#pragma once

#include <pch.h>
#include <common.h>

class RenderPass;

// VkImage + framebuffer wrapper
class RenderTarget {
public:
    VkImage colorImage{VK_NULL_HANDLE};
    VkImage depthImage{VK_NULL_HANDLE};
    VkImageView colorView{VK_NULL_HANDLE};
    VkImageView depthView{VK_NULL_HANDLE};
    VkFramebuffer fb{VK_NULL_HANDLE};

    RenderTarget() = default;

    RenderTarget(RenderTarget &&other) noexcept;
    RenderTarget &operator=(RenderTarget &&other) noexcept;
    RenderTarget(const RenderTarget &) = delete;
    RenderTarget &operator=(const RenderTarget &) = delete;

    ~RenderTarget();

    void Create(VkDevice device, VkImage aColorImage, VkImage aDepthImage, VkExtent2D size, RenderPass &renderPass);

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};

