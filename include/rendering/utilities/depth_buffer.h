/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/command_buffer.h>

namespace rvr {
class DepthBuffer {
public:
    VkDeviceMemory depthMemory_{VK_NULL_HANDLE};
    VkImage depthImage{VK_NULL_HANDLE};

    DepthBuffer() = default;
    ~DepthBuffer();
    DepthBuffer(DepthBuffer &&other) noexcept;
    DepthBuffer &operator=(DepthBuffer &&other) noexcept;

    DepthBuffer(const DepthBuffer &) = delete;
    DepthBuffer &operator=(const DepthBuffer &) = delete;

//    const std::shared_ptr<RenderingContext>& context;
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkFormat depthFormat,
                const XrSwapchainCreateInfo &swapchainCreateInfo);

    void TransitionLayout(CmdBuffer* cmdBuffer, VkImageLayout newLayout);

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    VkImageLayout m_vkLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};
}