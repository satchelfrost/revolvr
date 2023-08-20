#pragma once

#include <pch.h>
#include <ktx.h>
#include <rendering/utilities/rendering_context.h>
#include <rendering/utilities/vulkan_view.h>
#include <rendering/utilities/vulkan_image.h>

namespace rvr {
struct Texture {
    VkDevice device;
    std::shared_ptr<VulkanImage> image_;
    std::unique_ptr<VulkanView> view_;
    VkImageLayout imageLayout_;
    VkDeviceMemory deviceMemory;
    uint32_t width, height;
    uint32_t mipLevels;
    uint32_t layerCount;
    VkDescriptorImageInfo descriptor;
    VkSampler sampler;

    void UpdateDescriptor();
    void Destroy();
    void FromBuffer(
            void *             buffer,
            VkDeviceSize       bufferSize,
            VkFormat           format,
            uint32_t           texWidth,
            uint32_t           texHeight,
            const std::shared_ptr<RenderingContext>& renderingContext,
            VkFilter           filter          = VK_FILTER_LINEAR,
            VkImageUsageFlags  imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
            VkImageLayout      imageLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};
}