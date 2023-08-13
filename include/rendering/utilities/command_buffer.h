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
class CommandBuffer {
private:
    VkCommandBuffer commandBuffer_{VK_NULL_HANDLE};
    VkFence execFence{VK_NULL_HANDLE};
    VkCommandPool pool_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};

public:
    CommandBuffer(VkDevice device, VkCommandPool pool);
    VkCommandBuffer GetBuffer();
    ~CommandBuffer();
    void Begin();
    void End();
    void Exec(VkQueue queue);
    void Wait();
    void Reset();
};
}