/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include "rendering_context.h"

namespace rvr {
class CmdBuffer {
private:
    VkCommandBuffer commandBuffer_{VK_NULL_HANDLE};
    VkFence fence_{VK_NULL_HANDLE};
    VkCommandPool pool_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};

public:
    CmdBuffer(VkDevice device, VkCommandPool pool);
    VkCommandBuffer GetBuffer();
    ~CmdBuffer();
    void Begin();
    void End();
    void Exec(VkQueue queue);
    void Wait();
    void Reset();
};
}
