/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/render_pass.h>
#include <rendering//utilities/vulkan_results.h>
#include <array>

namespace rvr {
RenderPass::RenderPass(VkDevice device, VkFormat colorFmt, VkFormat depthFmt) : device_(device) {

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

    VkResult result = vkCreateRenderPass(device_, &rpInfo, nullptr, &pass_);
    CHECK_VKCMD(result);
}

RenderPass::~RenderPass() {
    if (device_ != nullptr) {
        if (pass_ != VK_NULL_HANDLE) {
            vkDestroyRenderPass(device_, pass_, nullptr);
        }
    }
    pass_ = VK_NULL_HANDLE;
    device_ = nullptr;
}

VkRenderPass RenderPass::GetRenderPass() {
    return pass_;
}
}