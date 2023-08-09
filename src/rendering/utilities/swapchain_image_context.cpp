/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/swapchain_image_context.h>

namespace rvr {
SwapchainImageContext::SwapchainImageContext(uint32_t capacity, const XrSwapchainCreateInfo &swapchainCreateInfo) :
swapchainImageFormat_(static_cast<VkFormat>(swapchainCreateInfo.format)),
swapchainExtent_({swapchainCreateInfo.width, swapchainCreateInfo.height}) {
    swapchainImages_.resize(capacity);
    renderTarget_.resize(capacity);

    viewport_.x = 0.0f;
    viewport_.y = static_cast<float>(swapchainExtent_.height);
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

void SwapchainImageContext::BindRenderTarget(uint32_t index, VkRenderPassBeginInfo *renderPassBeginInfo) {
    if (renderTarget_[index].fb == VK_NULL_HANDLE) {
        renderTarget_[index].Create(device_, swapchainImages_[index].image,
                                   depthBuffer_.depthImage, size_, renderPass_);
    }
    renderPassBeginInfo->renderPass = renderPass_.pass;
    renderPassBeginInfo->framebuffer = renderTarget_[index].fb;
    renderPassBeginInfo->renderArea.offset = {0, 0};
    renderPassBeginInfo->renderArea.extent = size_;
}

XrSwapchainImageBaseHeader *SwapchainImageContext::GetFirstImagePointer() {
    return reinterpret_cast<XrSwapchainImageBaseHeader*>(&swapchainImages_[0]);
}

void SwapchainImageContext::Draw(uint32_t imageIdx, uint32_t idxCount, std::shared_ptr<Pipeline> pipeline,
                                 const std::vector<math::Transform> &transforms) {

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

    BindRenderTarget(imageIdx, &renderPassBeginInfo);

    vkCmdBeginRenderPass(cmdBuffer_.buf, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmdBuffer_.buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      swapchainContext->GetPipeline());

    // Bind index and vertex buffers
    vkCmdBindIndexBuffer(cmdBuffer_.buf, drawBuffer_.idxBuf, 0, VK_INDEX_TYPE_UINT16);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmdBuffer_.buf, 0, 1, &drawBuffer_.vtxBuf, &offset);

    // Compute the view-projection transform.
    // Note all matrices (including OpenXR) are column-major, right-handed.
    const auto &pose = layerView.pose;

    glm::mat4 projectionMatrix = math::matrix::CreateProjectionFromXrFOV(layerView.fov, 0.05f, 100.0f);
    glm::mat4 poseMatrix = math::Pose(pose).ToMat4();
    glm::mat4 viewMatrix = glm::affineInverse(poseMatrix);
    glm::mat4 viewProjection = projectionMatrix * viewMatrix;

    // Render each cube
    for (const math::Transform &cube : cubes) {
        glm::mat4 modelMatrix = cube.ToMat4();
        glm::mat4 mvp = viewProjection * modelMatrix;
        vkCmdPushConstants(cmdBuffer_.buf, pipelineLayout_.layout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvp),
                           glm::value_ptr(mvp));
        vkCmdDrawIndexed(cmdBuffer_.buf, drawBuffer_.count.idx,
                         1, 0, 0, 0);
    }

    vkCmdEndRenderPass(cmdBuffer_.buf);

    cmdBuffer_.End();
    cmdBuffer_.Exec(graphicsQueue_);

    // XXX Should double-buffer the command buffers, for now just flush
    cmdBuffer_.Wait();
}

void SwapchainImageContext::InitResources() {
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    depthBuffer_.Create(physicalDevice_, device_, depthFormat, swapchainCreateInfo);
    renderPass_.Create(device_, colorFormat, depthFormat);
}
}