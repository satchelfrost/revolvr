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
private:
    VkRenderPass pass_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};

public:
    ~RenderPass();
    RenderPass(VkDevice device, VkFormat colorFmt, VkFormat depthFmt);
    VkRenderPass GetRenderPass();
};
}