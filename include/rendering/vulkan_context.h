/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>

#include "rendering/utilities/memory_allocator.h"
#include "rendering/utilities/command_buffer.h"
#include "rendering/utilities/shader_program.h"
#include "rendering/utilities/vertex_buffer.h"
#include "rendering/utilities/pipeline.h"
#include "rendering/utilities/depth_buffer.h"
#include <rendering/utilities/geometry.h>
#include <rendering/utilities/swapchain_image_context.h>

#include <platform/android_context.h>
#include <ecs/system/render_system.h>

namespace rvr {
class XrContext;

class VulkanContext {
public:
    static std::vector<std::string> GetInstanceExtensions();
    void InitializeDevice(XrInstance instance, XrSystemId systemId);
    std::vector<XrSwapchainImageBaseHeader*> AllocateSwapchainImageStructs(
            uint32_t capacity,
            const XrSwapchainCreateInfo& swapchainCreateInfo);
    static uint32_t GetSupportedSwapchainSampleCount(const XrViewConfigurationView& view);
    const XrBaseInStructure* GetGraphicsBinding() const;
    void Render();

private:
    void DrawGrid();
    void RenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage,
                    int64_t swapchainFormat, const std::vector<math::Transform>& cubes);
    bool RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                     XrCompositionLayerProjection& layer, XrContext* xrContext);

    std::vector<math::Transform> renderBuffer_;

    XrGraphicsBindingVulkan2KHR graphicsBinding_{XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR};
    std::list<SwapchainImageContext> swapchainImageContexts_;
    std::map<const XrSwapchainImageBaseHeader*, SwapchainImageContext*> swapchainImageContextMap_;

    VkInstance instance_{VK_NULL_HANDLE};
    VkPhysicalDevice physicalDevice_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};
    uint32_t queueFamilyIndex_ = 0;
    VkQueue graphicsQueue_{VK_NULL_HANDLE};
    VkSemaphore drawDone_{VK_NULL_HANDLE};

    MemoryAllocator memAllocator_{};
    ShaderProgram shaderProgram_{};
    CmdBuffer cmdBuffer_{};
    PipelineLayout pipelineLayout_{};
    VertexBuffer<Geometry::Vertex> drawBuffer_{};

    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT{nullptr};
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT{nullptr};
    VkDebugReportCallbackEXT debugReporter_{VK_NULL_HANDLE};

    static XrStructureType GetSwapchainImageType() ;
    void InitializeResources();
};
}