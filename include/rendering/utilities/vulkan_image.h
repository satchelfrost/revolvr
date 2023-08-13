#pragma once

#include <pch.h>
#include <rendering/utilities/rendering_context.h>

namespace rvr {
class VulkanImage {
public:
    enum ImageType {
        Depth,
        Color
    };

private:
    VkImage image_;
    VkDeviceMemory imageMemory_;
    VkDevice device_;

public:
    VulkanImage(const std::shared_ptr<RenderingContext>& context, ImageType imageType, VkExtent2D extent);
    VkImage GetImage();
    ~VulkanImage();
};
}