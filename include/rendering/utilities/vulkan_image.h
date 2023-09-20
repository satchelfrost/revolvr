#pragma once

#include <pch.h>
#include <rendering/utilities/rendering_context.h>

namespace rvr {
class VulkanImage {
public:
    enum ImageType {
        Depth,
        Color,
        Sample
    };

private:
    VkImage image_;
    VkDeviceMemory imageMemory_;
    VkDevice device_;

public:
    // TODO: definitely need to expose usage, probably also format. Maybe use some sane defaults.
    VulkanImage(const std::shared_ptr<RenderingContext>& context, ImageType imageType, VkExtent2D extent);
    VkImage GetImage();
    ~VulkanImage();
};
}