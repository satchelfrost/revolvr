#pragma once

#include <pch.h>
#include <common.h>
#include <array>

// RenderPass wrapper
struct RenderPass {
    VkFormat colorFmt{};
    VkFormat depthFmt{};
    VkRenderPass pass{VK_NULL_HANDLE};

    RenderPass() = default;

    bool Create(VkDevice device, VkFormat aColorFmt, VkFormat aDepthFmt) {
        m_vkDevice = device;
        colorFmt = aColorFmt;
        depthFmt = aDepthFmt;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkAttachmentReference colorRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        VkAttachmentReference depthRef = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

        std::array<VkAttachmentDescription, 2> at = {};

        VkRenderPassCreateInfo rpInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
        rpInfo.attachmentCount = 0;
        rpInfo.pAttachments = at.data();
        rpInfo.subpassCount = 1;
        rpInfo.pSubpasses = &subpass;

        if (colorFmt != VK_FORMAT_UNDEFINED) {
            colorRef.attachment = rpInfo.attachmentCount++;

            at[colorRef.attachment].format = colorFmt;
            at[colorRef.attachment].samples = VK_SAMPLE_COUNT_1_BIT;
            at[colorRef.attachment].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            at[colorRef.attachment].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            at[colorRef.attachment].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            at[colorRef.attachment].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            at[colorRef.attachment].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            at[colorRef.attachment].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorRef;
        }

        if (depthFmt != VK_FORMAT_UNDEFINED) {
            depthRef.attachment = rpInfo.attachmentCount++;

            at[depthRef.attachment].format = depthFmt;
            at[depthRef.attachment].samples = VK_SAMPLE_COUNT_1_BIT;
            at[depthRef.attachment].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            at[depthRef.attachment].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            at[depthRef.attachment].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            at[depthRef.attachment].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            at[depthRef.attachment].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            at[depthRef.attachment].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            subpass.pDepthStencilAttachment = &depthRef;
        }

        CHECK_VKCMD(vkCreateRenderPass(m_vkDevice, &rpInfo, nullptr, &pass));

        return true;
    }

    ~RenderPass() {
        if (m_vkDevice != nullptr) {
            if (pass != VK_NULL_HANDLE) {
                vkDestroyRenderPass(m_vkDevice, pass, nullptr);
            }
        }
        pass = VK_NULL_HANDLE;
        m_vkDevice = nullptr;
    }

    RenderPass(const RenderPass &) = delete;

    RenderPass &operator=(const RenderPass &) = delete;

    RenderPass(RenderPass &&) = delete;

    RenderPass &operator=(RenderPass &&) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};
