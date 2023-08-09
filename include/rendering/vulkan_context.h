/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>

#include "rendering/utilities/command_buffer.h"
#include "rendering/utilities/shader_program.h"
#include "rendering/utilities/draw_buffer.h"
#include "rendering/utilities/pipeline.h"
#include "rendering/utilities/depth_buffer.h"
#include <rendering/utilities/geometry.h>
#include <rendering/utilities/swapchain_image_context.h>
//#include "rendering/utilities/gltf/vulkan_gltf_model.h"

#include <rendering/utilities/rendering_context.h>

#include <platform/android_context.h>
#include <ecs/system/render_system.h>

namespace rvr {
class XrContext;

class VulkanContext {
private:
    VkInstance instance_{VK_NULL_HANDLE};
    VkDebugReportCallbackEXT debugReporter_{VK_NULL_HANDLE};
    const std::vector<const char*> requestedValidationLayers_ = {"VK_LAYER_KHRONOS_validation"};
    VkPhysicalDevice physicalDevice_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};
    VkQueue graphicsQueue_{VK_NULL_HANDLE};
    std::map<const XrSwapchainImageBaseHeader*, std::shared_ptr<SwapchainImageContext>> imageToContextMap_;
    std::vector<math::Transform> renderBuffer_;
    DepthBuffer depthBuffer_{};
    XrGraphicsBindingVulkan2KHR graphicsBinding_{XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR};
    VkSemaphore drawDone_{VK_NULL_HANDLE};
    ShaderProgram shaderProgram_{};
    CmdBuffer cmdBuffer_{};
    Pipeline pipeline_{};
    RenderPass renderPass_{};
    DrawBuffer drawBuffer_{};
    std::shared_ptr<RenderingContext> renderingContext_ = nullptr;


#if !defined(NDEBUG)
        const bool enableValidationLayers_ = true;
#else
        const bool enableValidationLayers_ = false;
#endif

public:
    void InitDevice(XrInstance xrInstance, XrSystemId systemId);
    void Cleanup();
    XrSwapchainImageBaseHeader* AllocateSwapchainImageStructs(uint32_t capacity,
                                                              const XrSwapchainCreateInfo& swapchainCreateInfo);
    static std::vector<std::string> GetInstanceExtensions();
    static uint32_t GetSupportedSwapchainSampleCount(const XrViewConfigurationView& view);
    const XrBaseInStructure* GetGraphicsBinding() const;
    void Render();
    void InitializeResources();
    void InitRenderingContext(VkFormat colorFormat);
    void SwapchainImagesReady(XrSwapchainImageBaseHeader* images);

private:
    void CreateVulkanInstance(XrInstance xrInstance, XrSystemId systemId);
    bool CheckValidationLayerSupport();
    void SetupReportCallback();
    void PickPhysicalDevice(XrInstance xrInstance, XrSystemId systemId);
    void CreateLogicalDevice(XrInstance xrInstance, XrSystemId systemId);
    void StoreGraphicsBinding();
    void DrawGrid();
    void RenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage,
                    const uint32_t imageIndex, const std::vector<math::Transform>& cubes);
    bool RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                     XrCompositionLayerProjection& layer, XrContext* xrContext);
    void RetrieveQueues();
};
}