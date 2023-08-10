/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/render_target.h>
#include <rendering/utilities/render_pass.h>
#include <rendering/utilities/vulkan_results.h>
#include <array>

namespace rvr {
RenderTarget::~RenderTarget() {
    if (device_ != nullptr) {
        if (framebuffer_ != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device_, framebuffer_, nullptr);
        }
        if (colorView_ != VK_NULL_HANDLE) {
            vkDestroyImageView(device_, colorView_, nullptr);
        }
        if (depthView_ != VK_NULL_HANDLE) {
            vkDestroyImageView(device_, depthView_, nullptr);
        }
    }

    // Note we don't own color/depthImage, it will get destroyed when xrDestroySwapchain is called
    colorImage_ = VK_NULL_HANDLE;
    depthImage_ = VK_NULL_HANDLE;
    colorView_ = VK_NULL_HANDLE;
    depthView_ = VK_NULL_HANDLE;
    framebuffer_ = VK_NULL_HANDLE;
    device_ = nullptr;
}

RenderTarget::RenderTarget(RenderTarget &&other) noexcept: RenderTarget() {
    using std::swap;
    swap(colorImage_, other.colorImage_);
    swap(depthImage_, other.depthImage_);
    swap(colorView_, other.colorView_);
    swap(depthView_, other.depthView_);
    swap(framebuffer_, other.framebuffer_);
    swap(device_, other.device_);
}

RenderTarget &RenderTarget::operator=(RenderTarget &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    // Clean up ourselves.
    this->~RenderTarget();
    using std::swap;
    swap(colorImage_, other.colorImage_);
    swap(depthImage_, other.depthImage_);
    swap(colorView_, other.colorView_);
    swap(depthView_, other.depthView_);
    swap(framebuffer_, other.framebuffer_);
    swap(device_, other.device_);
    return *this;
}

void RenderTarget::Create(const std::shared_ptr<RenderingContext>& context, VkImage colorImage, VkImage depthImage,
                          VkExtent2D size) {
    device_ = context->GetDevice();

    colorImage_ = colorImage;
    depthImage_ = depthImage;

    std::array<VkImageView, 2> attachments{};
    uint32_t attachmentCount = 0;

    // Create color image view
    if (colorImage_ != VK_NULL_HANDLE) {
        VkImageViewCreateInfo colorViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        colorViewInfo.image = colorImage_;
        colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorViewInfo.format = context->GetColorFormat();
        colorViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        colorViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        colorViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        colorViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorViewInfo.subresourceRange.baseMipLevel = 0;
        colorViewInfo.subresourceRange.levelCount = 1;
        colorViewInfo.subresourceRange.baseArrayLayer = 0;
        colorViewInfo.subresourceRange.layerCount = 1;
        CHECK_VKCMD(vkCreateImageView(device_, &colorViewInfo, nullptr, &colorView_));
        attachments[attachmentCount++] = colorView_;
    }

    // Create depth image view
    if (depthImage_ != VK_NULL_HANDLE) {
        VkImageViewCreateInfo depthViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        depthViewInfo.image = depthImage_;
        depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthViewInfo.format = context->GetDepthFormat();
        depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        depthViewInfo.subresourceRange.baseMipLevel = 0;
        depthViewInfo.subresourceRange.levelCount = 1;
        depthViewInfo.subresourceRange.baseArrayLayer = 0;
        depthViewInfo.subresourceRange.layerCount = 1;
        CHECK_VKCMD(vkCreateImageView(device_, &depthViewInfo, nullptr, &depthView_));
        attachments[attachmentCount++] = depthView_;
    }

    VkFramebufferCreateInfo fbInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    fbInfo.renderPass = context->GetRenderPass();
    fbInfo.attachmentCount = attachmentCount;
    fbInfo.pAttachments = attachments.data();
    fbInfo.width = size.width;
    fbInfo.height = size.height;
    fbInfo.layers = 1;
    CHECK_VKCMD(vkCreateFramebuffer(device_, &fbInfo, nullptr, &framebuffer_));
}

VkFramebuffer RenderTarget::GetFramebuffer() {
    return framebuffer_;
}
}