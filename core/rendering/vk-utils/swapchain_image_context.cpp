/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include "swapchain_image_context.h"
#include "global_context.h"
#include <utility>

namespace rvr {
SwapchainImageContext::SwapchainImageContext(const std::shared_ptr<RenderingContext>& renderingContext,
                                             uint32_t capacity, const XrSwapchainCreateInfo &swapchainCreateInfo) :
renderingContext_(renderingContext), swapchainExtent_({swapchainCreateInfo.width, swapchainCreateInfo.height}) {
    swapchainImages_.resize(capacity);
    renderTargets_.resize(capacity);

    cmdBuffs_.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        cmdBuffs_[i] = std::make_unique<CmdBuffer>(renderingContext_->GetDevice(), renderingContext->GetGraphicsPool());
    }
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
    VkCommandBuffer cmdBuffer = cmdBuffs_[currFrame]->GetBuffer();
    pipeline->BindPipeline(cmdBuffer);
    vkCmdBindIndexBuffer(cmdBuffer, drawBuffer->GetIndexBuffer(),
                         0, VK_INDEX_TYPE_UINT16);
    VkDeviceSize offset = 0;
    VkBuffer vertexBuffer = drawBuffer->GetVertexBuffer();
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer,
                           &offset);
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport_);
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor_);
    for (const auto& transform : transforms) {
        vkCmdPushConstants(cmdBuffer, pipeline->GetPipelineLayout(),
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(transform),
                           glm::value_ptr(transform));
        vkCmdDrawIndexed(cmdBuffer, drawBuffer->GetIndexCount(),
                         1, 0, 0, 0);
    }
}

void SwapchainImageContext::DrawGltf(const std::unique_ptr<Pipeline>& pipeline,
                                     const std::unique_ptr<GLTFModel>& model, VkDescriptorSet descriptorSet) {
    VkCommandBuffer cmdBuffer = cmdBuffs_[currFrame]->GetBuffer();
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport_);
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor_);
    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline->GetPipelineLayout(), 0, 1,
                            &descriptorSet, 0, nullptr) ;
    pipeline->BindPipeline(cmdBuffer);
    model->Draw(cmdBuffer, pipeline->GetPipelineLayout());
}

void SwapchainImageContext::DrawPointCloud(const std::unique_ptr<Pipeline>& pipeline,
                                           const std::unique_ptr<PointCloudResource>& pointCloud) {
    VkCommandBuffer cmdBuffer = cmdBuffs_[currFrame]->GetBuffer();
    pipeline->BindPipeline(cmdBuffer);
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport_);
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor_);
    pointCloud->Draw(cmdBuffer, pipeline->GetPipelineLayout());
}

void SwapchainImageContext::InitRenderTargets() {
    for (size_t i = 0; i < swapchainImages_.size(); i++)
        renderTargets_[i] = std::make_unique<RenderTarget>(renderingContext_,
                                                           swapchainImages_[i].image,
                                                           swapchainExtent_);
}

void SwapchainImageContext::BeginRenderPass(uint32_t imageIndex) {
    cmdBuffs_[currFrame]->Wait();
    cmdBuffs_[currFrame]->Reset();
    cmdBuffs_[currFrame]->Begin();

    static XrColor4f clrColor = {0.184313729f, 0.309803933f, 0.309803933f, 1.0f};
    if (GlobalContext::Inst()->ExtMan()->usingPassthrough_)
        clrColor = {0.0f, 0.0f, 0.0f, 0.0f};

    static std::array<VkClearValue, 2> clearValues;
    clearValues[0].color.float32[0] = clrColor.r;
    clearValues[0].color.float32[1] = clrColor.g;
    clearValues[0].color.float32[2] = clrColor.b;
    clearValues[0].color.float32[3] = clrColor.a;
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;
    VkRenderPassBeginInfo renderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    renderPassBeginInfo.clearValueCount = (uint32_t) clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();
    renderPassBeginInfo.renderPass = renderingContext_->GetRenderPass();
    renderPassBeginInfo.framebuffer = renderTargets_[imageIndex]->GetFramebuffer();
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = swapchainExtent_;
    vkCmdBeginRenderPass(cmdBuffs_[currFrame]->GetBuffer(), &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
}

void SwapchainImageContext::EndRenderPass() {
    vkCmdEndRenderPass(cmdBuffs_[currFrame]->GetBuffer());
    cmdBuffs_[currFrame]->End();
    cmdBuffs_[currFrame]->Exec(renderingContext_->GetGraphicsQueue());
    currFrame = (currFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
}
