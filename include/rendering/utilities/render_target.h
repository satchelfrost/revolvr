#pragma once

#include <pch.h>
#include <common.h>

// VkImage + framebuffer wrapper
struct RenderTarget {
    VkImage colorImage{VK_NULL_HANDLE};
    VkImage depthImage{VK_NULL_HANDLE};
    VkImageView colorView{VK_NULL_HANDLE};
    VkImageView depthView{VK_NULL_HANDLE};
    VkFramebuffer fb{VK_NULL_HANDLE};

    RenderTarget() = default;

    ~RenderTarget() {
        if (m_vkDevice != nullptr) {
            if (fb != VK_NULL_HANDLE) {
                vkDestroyFramebuffer(m_vkDevice, fb, nullptr);
            }
            if (colorView != VK_NULL_HANDLE) {
                vkDestroyImageView(m_vkDevice, colorView, nullptr);
            }
            if (depthView != VK_NULL_HANDLE) {
                vkDestroyImageView(m_vkDevice, depthView, nullptr);
            }
        }

        // Note we don't own color/depthImage, it will get destroyed when xrDestroySwapchain is called
        colorImage = VK_NULL_HANDLE;
        depthImage = VK_NULL_HANDLE;
        colorView = VK_NULL_HANDLE;
        depthView = VK_NULL_HANDLE;
        fb = VK_NULL_HANDLE;
        m_vkDevice = nullptr;
    }

    RenderTarget(RenderTarget &&other) noexcept: RenderTarget() {
            using std::swap;
            swap(colorImage, other.colorImage);
            swap(depthImage, other.depthImage);
            swap(colorView, other.colorView);
            swap(depthView, other.depthView);
            swap(fb, other.fb);
            swap(m_vkDevice, other.m_vkDevice);
    }

    RenderTarget &operator=(RenderTarget &&other) noexcept {
        if (&other == this) {
            return *this;
        }
        // Clean up ourselves.
        this->~RenderTarget();
        using std::swap;
        swap(colorImage, other.colorImage);
        swap(depthImage, other.depthImage);
        swap(colorView, other.colorView);
        swap(depthView, other.depthView);
        swap(fb, other.fb);
        swap(m_vkDevice, other.m_vkDevice);
        return *this;
    }

    void Create(VkDevice device, VkImage aColorImage, VkImage aDepthImage, VkExtent2D size,
                RenderPass &renderPass) {
        m_vkDevice = device;

        colorImage = aColorImage;
        depthImage = aDepthImage;

        std::array<VkImageView, 2> attachments{};
        uint32_t attachmentCount = 0;

        // Create color image view
        if (colorImage != VK_NULL_HANDLE) {
            VkImageViewCreateInfo colorViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            colorViewInfo.image = colorImage;
            colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            colorViewInfo.format = renderPass.colorFmt;
            colorViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            colorViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            colorViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            colorViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorViewInfo.subresourceRange.baseMipLevel = 0;
            colorViewInfo.subresourceRange.levelCount = 1;
            colorViewInfo.subresourceRange.baseArrayLayer = 0;
            colorViewInfo.subresourceRange.layerCount = 1;
            CHECK_VKCMD(vkCreateImageView(m_vkDevice, &colorViewInfo, nullptr, &colorView));
            attachments[attachmentCount++] = colorView;
        }

        // Create depth image view
        if (depthImage != VK_NULL_HANDLE) {
            VkImageViewCreateInfo depthViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            depthViewInfo.image = depthImage;
            depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            depthViewInfo.format = renderPass.depthFmt;
            depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthViewInfo.subresourceRange.baseMipLevel = 0;
            depthViewInfo.subresourceRange.levelCount = 1;
            depthViewInfo.subresourceRange.baseArrayLayer = 0;
            depthViewInfo.subresourceRange.layerCount = 1;
            CHECK_VKCMD(vkCreateImageView(m_vkDevice, &depthViewInfo, nullptr, &depthView));
            attachments[attachmentCount++] = depthView;
        }

        VkFramebufferCreateInfo fbInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        fbInfo.renderPass = renderPass.pass;
        fbInfo.attachmentCount = attachmentCount;
        fbInfo.pAttachments = attachments.data();
        fbInfo.width = size.width;
        fbInfo.height = size.height;
        fbInfo.layers = 1;
        CHECK_VKCMD(vkCreateFramebuffer(m_vkDevice, &fbInfo, nullptr, &fb));
    }

    RenderTarget(const RenderTarget &) = delete;

    RenderTarget &operator=(const RenderTarget &) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};

