/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "render_target.h"
#include "render_pass.h"
#include "vk_results.h"
#include "cmd_buffer.h"
#include <array>

namespace rvr {
RenderTarget::RenderTarget(const std::shared_ptr<RenderingContext>& context, VkImage colorImage, VkExtent2D extent) {
    device_ = context->GetDevice();
    colorView_ = new View(context, View::Color, colorImage);
    depthImage_ = new Image(context, Image::Depth, extent);
    depthView_ = new View(context, View::Depth, depthImage_->GetImage());

    CmdBuffer cmd = CmdBuffer(context->GetDevice(), context->GetGraphicsPool());
    cmd.Begin();
    context->CreateTransitionLayout(cmd.GetBuffer(), depthImage_->GetImage(),
                                    VK_IMAGE_LAYOUT_UNDEFINED,
                                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    cmd.End();
    cmd.Exec(context->GetGraphicsQueue());

    std::array<VkImageView, 2> attachments{colorView_->GetImageView(), depthView_->GetImageView()};
    VkFramebufferCreateInfo fbInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    fbInfo.renderPass = context->GetRenderPass();
    fbInfo.attachmentCount = attachments.size();
    fbInfo.pAttachments = attachments.data();
    fbInfo.width = extent.width;
    fbInfo.height = extent.height;
    fbInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(device_, &fbInfo, nullptr, &framebuffer_);
    CHECK_VKCMD(result);
}

RenderTarget::~RenderTarget() {
    if (device_ != nullptr) {
        if (framebuffer_ != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device_, framebuffer_, nullptr);
        }
    }

    // Delete views before images
    delete depthView_;
    delete colorView_;
    delete depthImage_;
}

VkFramebuffer RenderTarget::GetFramebuffer() {
    return framebuffer_;
}
}
