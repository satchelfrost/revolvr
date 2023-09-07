/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/swapchain_image_context.h>

#include <utility>

namespace rvr {
SwapchainImageContext::SwapchainImageContext(const std::shared_ptr<RenderingContext>& renderingContext,
                                             uint32_t capacity, const XrSwapchainCreateInfo &swapchainCreateInfo) :
renderingContext_(renderingContext), swapchainExtent_({swapchainCreateInfo.width, swapchainCreateInfo.height}) {
    swapchainImages_.resize(capacity);
    renderTargets_.resize(capacity);

    cmdBuffer_ = std::make_unique<CommandBuffer>(renderingContext_->GetDevice(),
                                                 renderingContext->GetGraphicsPool());
    viewport_.x = 0.0f;
    viewport_.y = 0.0f;
    viewport_.width = static_cast<float>(swapchainExtent_.width);
    viewport_.height = static_cast<float>(swapchainExtent_.height);
    viewport_.minDepth = 0.0f;
    viewport_.maxDepth = 1.0f;

    scissor_.offset = {0, 0};
    scissor_.extent.width = static_cast<uint32_t>(swapchainExtent_.width);
    scissor_.extent.height = static_cast<uint32_t>(swapchainExtent_.height);

    for (auto& image : swapchainImages_ )
        image.type = XR_TYPE_SWAPCHAIN_IMAGE_VULKAN2_KHR;
}

XrSwapchainImageBaseHeader *SwapchainImageContext::GetFirstImagePointer() {
    return reinterpret_cast<XrSwapchainImageBaseHeader*>(&swapchainImages_[0]);
}

void SwapchainImageContext::Draw(const std::unique_ptr<Pipeline>& pipeline,
                                 const std::unique_ptr<DrawBuffer>& drawBuffer,
                                 const std::vector<glm::mat4> &transforms) {
    pipeline->BindPipeline(cmdBuffer_->GetBuffer());
    vkCmdBindIndexBuffer(cmdBuffer_->GetBuffer(), drawBuffer->GetIndexBuffer(),
                         0, VK_INDEX_TYPE_UINT16);
    VkDeviceSize offset = 0;
    VkBuffer vertexBuffer = drawBuffer->GetVertexBuffer();
    vkCmdBindVertexBuffers(cmdBuffer_->GetBuffer(), 0, 1, &vertexBuffer,
                           &offset);
    vkCmdSetViewport(cmdBuffer_->GetBuffer(), 0, 1, &viewport_);
    vkCmdSetScissor(cmdBuffer_->GetBuffer(), 0, 1, &scissor_);
    for (const auto& transform : transforms) {
        vkCmdPushConstants(cmdBuffer_->GetBuffer(), pipeline->GetPipelineLayout(),
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(transform),
                           glm::value_ptr(transform));
        vkCmdDrawIndexed(cmdBuffer_->GetBuffer(), drawBuffer->GetIndexCount(),
                         1, 0, 0, 0);
    }
}

void SwapchainImageContext::DrawGltf(const std::unique_ptr<Pipeline>& pipeline,
                                     const std::unique_ptr<VulkanGLTFModel>& model, VkDescriptorSet descriptorSet) {
    vkCmdSetViewport(cmdBuffer_->GetBuffer(), 0, 1, &viewport_);
    vkCmdSetScissor(cmdBuffer_->GetBuffer(), 0, 1, &scissor_);
    vkCmdBindDescriptorSets(cmdBuffer_->GetBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline->GetPipelineLayout(), 0, 1,
                            &descriptorSet, 0, nullptr) ;
    pipeline->BindPipeline(cmdBuffer_->GetBuffer());
    model->Draw(cmdBuffer_->GetBuffer(), pipeline->GetPipelineLayout());
}

void SwapchainImageContext::InitRenderTargets() {
    for (size_t i = 0; i < swapchainImages_.size(); i++)
        renderTargets_[i] = std::make_unique<RenderTarget>(renderingContext_,
                                                           swapchainImages_[i].image,
                                                           swapchainExtent_);
}

void SwapchainImageContext::BeginRenderPass(uint32_t imageIndex) {
    cmdBuffer_->Wait();
    cmdBuffer_->Reset();
    cmdBuffer_->Begin();

    // Bind and clear eye render target
    static XrColor4f darkSlateGrey = {0.184313729f, 0.309803933f, 0.309803933f, 1.0f};
    static std::array<VkClearValue, 2> clearValues;
    clearValues[0].color.float32[0] = darkSlateGrey.r;
    clearValues[0].color.float32[1] = darkSlateGrey.g;
    clearValues[0].color.float32[2] = darkSlateGrey.b;
    clearValues[0].color.float32[3] = darkSlateGrey.a;
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;
    VkRenderPassBeginInfo renderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    renderPassBeginInfo.clearValueCount = (uint32_t) clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();
    renderPassBeginInfo.renderPass = renderingContext_->GetRenderPass();
    renderPassBeginInfo.framebuffer = renderTargets_[imageIndex]->GetFramebuffer();
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = swapchainExtent_;
    vkCmdBeginRenderPass(cmdBuffer_->GetBuffer(), &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
}

void SwapchainImageContext::EndRenderPass() {
    vkCmdEndRenderPass(cmdBuffer_->GetBuffer());
    cmdBuffer_->End();
    cmdBuffer_->Exec(renderingContext_->GetGraphicsQueue());
}
}