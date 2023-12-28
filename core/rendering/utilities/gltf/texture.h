/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>
#include <ktx.h>
#include <rendering/utilities/rendering_context.h>
#include <rendering/utilities/gltf/vulkan_sampler.h>
#include <rendering/utilities/vulkan_view.h>
#include <rendering/utilities/vulkan_image.h>

namespace rvr {
struct VulkanTexture {
    VkDevice device_;
    std::shared_ptr<VulkanImage> image_;
    std::unique_ptr<VulkanView> view_;
    std::unique_ptr<VulkanSampler> sampler_;
    VkImageLayout imageLayout_;
    uint32_t width, height;
    uint32_t mipLevels;
    uint32_t layerCount;
    VkDescriptorImageInfo descriptor;

    void UpdateDescriptor();
    void Destroy();
    void FromBuffer(
            void *             buffer,
            VkDeviceSize       bufferSize,
            uint32_t           texWidth,
            uint32_t           texHeight,
            const std::shared_ptr<RenderingContext>& renderingContext,
            VkFilter           filter          = VK_FILTER_LINEAR,
            VkImageLayout      imageLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};
}