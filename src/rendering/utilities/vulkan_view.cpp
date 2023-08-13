#include <rendering/utilities/vulkan_view.h>

namespace rvr {
VulkanView::VulkanView(const std::shared_ptr<RenderingContext>& context, ViewType viewType, VkImage image) :
device_(context->GetDevice()) {
    VkFormat format;
    VkImageAspectFlagBits aspectMask;
    if (viewType == ViewType::Depth) {
        format = context->GetDepthFormat();
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    else if (viewType == ViewType::Color) {
        format = context->GetColorFormat();
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    else
        THROW("Image type undefined");

    context->CreateImageView(image, format, aspectMask, &imageView_);
}


VulkanView::~VulkanView() {
    vkDestroyImageView(device_, imageView_, nullptr);
}

VkImageView VulkanView::GetImageView() {
    return imageView_;
}
}
