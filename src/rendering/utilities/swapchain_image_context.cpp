/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#include <rendering/utilities/swapchain_image_context.h>
#include <rendering/utilities/memory_allocator.h>

SwapchainImageContext::SwapchainImageContext(XrStructureType _swapchainImageType)
: swapchainImageType(_swapchainImageType) {}

std::vector<XrSwapchainImageBaseHeader *> SwapchainImageContext::Create(VkDevice device,
                              MemoryAllocator *memAllocator,
                              uint32_t capacity,
                              const XrSwapchainCreateInfo &swapchainCreateInfo,
                              const PipelineLayout &layout,
                              const ShaderProgram &sp,
                              const VertexBuffer<Geometry::Vertex> &vb) {
    m_vkDevice = device;

    size = {swapchainCreateInfo.width, swapchainCreateInfo.height};
    VkFormat colorFormat = (VkFormat) swapchainCreateInfo.format;
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    // XXX handle swapchainCreateInfo.sampleCount

    depthBuffer.Create(m_vkDevice, memAllocator, depthFormat, swapchainCreateInfo);
    rp.Create(m_vkDevice, colorFormat, depthFormat);
    pipe.Create(m_vkDevice, size, layout, rp, sp, vb);

    swapchainImages.resize(capacity);
    renderTarget.resize(capacity);
    std::vector < XrSwapchainImageBaseHeader * > bases(capacity);
    for (uint32_t i = 0; i < capacity; ++i) {
        swapchainImages[i] = {swapchainImageType};
        bases[i] = reinterpret_cast<XrSwapchainImageBaseHeader *>(&swapchainImages[i]);
    }

    return bases;
}

uint32_t SwapchainImageContext::ImageIndex(const XrSwapchainImageBaseHeader *swapchainImageHeader) {
    auto p = reinterpret_cast<const XrSwapchainImageVulkan2KHR *>(swapchainImageHeader);
    return (uint32_t)(p - &swapchainImages[0]);
}

void
SwapchainImageContext::BindRenderTarget(uint32_t index, VkRenderPassBeginInfo *renderPassBeginInfo) {
    if (renderTarget[index].fb == VK_NULL_HANDLE) {
        renderTarget[index].Create(m_vkDevice, swapchainImages[index].image,
                                   depthBuffer.depthImage, size, rp);
    }
    renderPassBeginInfo->renderPass = rp.pass;
    renderPassBeginInfo->framebuffer = renderTarget[index].fb;
    renderPassBeginInfo->renderArea.offset = {0, 0};
    renderPassBeginInfo->renderArea.extent = size;
}
