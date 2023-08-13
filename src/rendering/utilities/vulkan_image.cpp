#include <rendering/utilities/vulkan_image.h>

namespace rvr {
VulkanImage::VulkanImage(const std::shared_ptr<RenderingContext>& context, VulkanImage::ImageType imageType,
                         VkExtent2D extent) :
device_(context->GetDevice()) {
    VkFormat format;
    VkImageUsageFlags usage;
    if (imageType == ImageType::Depth) {
        format = context->GetDepthFormat();
        usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    else if (imageType == ImageType::Color) {
        format = context->GetColorFormat();
        usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    else
        THROW("Image type undefined");

    context->CreateImage(extent, format, usage, &image_, &imageMemory_);
}


VulkanImage::~VulkanImage() {
    vkDestroyImage(device_, image_, nullptr);
    vkFreeMemory(device_, imageMemory_, nullptr);
}

VkImage VulkanImage::GetImage() {
    return image_;
}
}
