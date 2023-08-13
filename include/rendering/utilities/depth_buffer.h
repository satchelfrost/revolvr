/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <common.h>
#include <rendering/utilities/command_buffer.h>
#include "swapchain_image_context.h"

namespace rvr {
class DepthBuffer {
private:
    VkDeviceMemory depthMemory_{VK_NULL_HANDLE};
    VkImage depthImage_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};
    VkImageLayout imageLayout_ = VK_IMAGE_LAYOUT_UNDEFINED;
    std::shared_ptr<VulkanBuffer> buffer_;

public:
    DepthBuffer(const std::shared_ptr<RenderingContext>& context, VkExtent2D extent,
                VkSampleCountFlagBits samples, VkCommandBuffer cmdBuffer, VkImageLayout newLayout);
    void TransitionLayout(VkCommandBuffer cmdBuffer, VkImageLayout newLayout);
    DepthBuffer() = default;
    ~DepthBuffer();
    DepthBuffer(DepthBuffer &&other) noexcept;
    DepthBuffer &operator=(DepthBuffer &&other) noexcept;
    DepthBuffer(const DepthBuffer &) = delete;
    DepthBuffer &operator=(const DepthBuffer &) = delete;
};
}