#pragma once

#include <pch.h>
#include <rendering/utilities/rendering_context.h>

namespace rvr {
class VulkanView {
public:
    enum ViewType {
        Depth,
        Color
    };

private:
    VkImageView imageView_;
    VkDevice device_;

public:
    VulkanView(const std::shared_ptr<RenderingContext>& context, ViewType viewType, VkImage image);
    VkImageView GetImageView();
    ~VulkanView();
};
}
