/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>

namespace rvr {
class RenderPass {
public:
    VkFormat colorFmt{};
    VkFormat depthFmt{};
    VkRenderPass pass{VK_NULL_HANDLE};

    RenderPass() = default;

    RenderPass(const RenderPass &) = delete;
    RenderPass &operator=(const RenderPass &) = delete;
    RenderPass(RenderPass &&) = delete;
    RenderPass &operator=(RenderPass &&) = delete;

    ~RenderPass();

    bool Create(VkDevice device, VkFormat aColorFmt, VkFormat aDepthFmt);

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};
}