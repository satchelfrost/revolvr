/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/swapchain_image_context.h>

namespace rvr {
SwapchainImageContext::SwapchainImageContext(XrStructureType swapchainImageType) :
swapchainImageType_(swapchainImageType) {}

void SwapchainImageContext::Create(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t capacity,
                                   const XrSwapchainCreateInfo &swapchainCreateInfo, const PipelineLayout &layout,
                                   const ShaderProgram &sp, const VertexBuffer<Geometry::Vertex> &vb) {
    device_ = device;
    size_ = {swapchainCreateInfo.width, swapchainCreateInfo.height};
    VkFormat colorFormat = (VkFormat) swapchainCreateInfo.format;
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

    depthBuffer_.Create(physicalDevice, device_, depthFormat, swapchainCreateInfo);
    renderPass_.Create(device_, colorFormat, depthFormat);
    pipeline_.Create(device_, size_, layout, renderPass_, sp, vb);

    swapchainImages_.resize(capacity);
    renderTarget_.resize(capacity);
    std::vector <XrSwapchainImageBaseHeader*> bases(capacity);
    for (uint32_t i = 0; i < capacity; ++i) {
        swapchainImages_[i] = {swapchainImageType_};
        bases[i] = reinterpret_cast<XrSwapchainImageBaseHeader*>(&swapchainImages_[i]);
    }
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

void SwapchainImageContext::TransitionLayout(CmdBuffer *cmdBuffer, VkImageLayout imageLayout) {
    depthBuffer_.TransitionLayout(cmdBuffer, imageLayout);
}

VkPipeline SwapchainImageContext::GetPipeline() {
    return pipeline_.pipe;
}
}