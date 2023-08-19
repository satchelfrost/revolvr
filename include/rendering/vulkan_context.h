/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <pch.h>

#include "rendering/utilities/shader_program.h"
#include "rendering/utilities/draw_buffer.h"
#include "rendering/utilities/pipeline.h"
#include "rendering/utilities/vulkan_utils.h"
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
    VkCommandPool graphicsPool_;
    std::map<const XrSwapchainImageBaseHeader*, std::shared_ptr<SwapchainImageContext>> imageToSwapchainContext_;
    std::vector<math::Transform> renderBuffer_;
    XrGraphicsBindingVulkan2KHR graphicsBinding_{XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR};
    std::unique_ptr<ShaderProgram> shaderProgram_;
    std::shared_ptr<DrawBuffer> drawBuffer_;
    std::unique_ptr<Pipeline> pipeline_;
    std::shared_ptr<RenderingContext> renderingContext_;
    QueueFamilyIndices queueFamilyIndices_;


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
    void InitializeResources();
    void InitRenderingContext(VkFormat colorFormat);
    void SwapchainImagesReady(XrSwapchainImageBaseHeader* images);
    void RenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage,
                    const uint32_t imageIndex);

private:
    void CreateVulkanInstance(XrInstance xrInstance, XrSystemId systemId);
    bool CheckValidationLayerSupport();
    void SetupReportCallback();
    void PickPhysicalDevice(XrInstance xrInstance, XrSystemId systemId);
    void CreateLogicalDevice(XrInstance xrInstance, XrSystemId systemId);
    void CreateCommandPool();
    void StoreGraphicsBinding();
    void RetrieveQueues();
};
}