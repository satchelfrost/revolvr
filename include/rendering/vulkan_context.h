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
struct Cube {
    XrPosef Pose;
    XrVector3f Scale;
};

class XrContext;

// Wraps a Vulkan API.
class VulkanContext {
public:
    VulkanContext();

    // OpenXR extensions required by this graphics API.
    virtual std::vector<std::string> GetInstanceExtensions() const;

    // Create an instance of this graphics api for the provided instance and systemId.
    virtual void InitializeDevice(XrInstance instance, XrSystemId systemId);

    // Select the preferred swapchain format from the list of available formats.
    virtual int64_t SelectColorSwapchainFormat(const std::vector<int64_t>& runtimeFormats) const;

    // Get the graphics binding header for session creation.
    virtual const XrBaseInStructure* GetGraphicsBinding() const;

    // Allocate space for the swapchain image structures. These are different for each graphics API. The returned
    // pointers are valid for the lifetime of the graphics plugin.
    virtual std::vector<XrSwapchainImageBaseHeader*> AllocateSwapchainImageStructs(
        uint32_t capacity, const XrSwapchainCreateInfo& swapchainCreateInfo);

    // Render to a swapchain image for a projection view.
    virtual void RenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage,
                            int64_t swapchainFormat, const std::vector<math::Transform>& cubes);

    // Get recommended number of sub-data element samples in view (recommendedSwapchainSampleCount)
    // if supported by the graphics plugin. A supported value otherwise.
    virtual uint32_t GetSupportedSwapchainSampleCount(const XrViewConfigurationView& view);

    void Render();

private:
    void DrawGrid(); // TODO: Remove once rendering is reworked

    bool RenderLayer(std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                     XrCompositionLayerProjection& layer, XrContext* xrContext);

private:
    std::vector<math::Transform> renderBuffer_;

    XrGraphicsBindingVulkan2KHR m_graphicsBinding{XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR};
    std::list<SwapchainImageContext> m_swapchainImageContexts;
    std::map<const XrSwapchainImageBaseHeader*, SwapchainImageContext*> m_swapchainImageContextMap;

    VkInstance m_vkInstance{VK_NULL_HANDLE};
    VkPhysicalDevice m_vkPhysicalDevice{VK_NULL_HANDLE};
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    uint32_t m_queueFamilyIndex = 0;
    VkQueue m_vkQueue{VK_NULL_HANDLE};
    VkSemaphore m_vkDrawDone{VK_NULL_HANDLE};

    MemoryAllocator m_memAllocator{};
    ShaderProgram m_shaderProgram{};
    CmdBuffer m_cmdBuffer{};
    PipelineLayout m_pipelineLayout{};
    VertexBuffer<Geometry::Vertex> m_drawBuffer{};

    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT{nullptr};
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT{nullptr};
    VkDebugReportCallbackEXT m_vkDebugReporter{VK_NULL_HANDLE};

    VkBool32 debugReport(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t /*location*/,
                         int32_t /*messageCode*/, const char* pLayerPrefix, const char* pMessage);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportThunk(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
                                                           uint64_t object, size_t location, int32_t messageCode,
                                                           const char* pLayerPrefix, const char* pMessage, void* pUserData);

    virtual XrStructureType GetGraphicsBindingType() const;
    virtual XrStructureType GetSwapchainImageType() const;

    virtual XrResult CreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR* createInfo,
                                             VkInstance* vulkanInstance, VkResult* vulkanResult);

    virtual XrResult CreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR* createInfo,
                                           VkDevice* vulkanDevice, VkResult* vulkanResult);

    virtual XrResult GetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR* getInfo,
                                                 VkPhysicalDevice* vulkanPhysicalDevice);

    virtual XrResult GetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId,
                                                       XrGraphicsRequirementsVulkan2KHR* graphicsRequirements);

    const char *GetValidationLayerName();

    std::vector<const char *> ParseExtensionString(char *names);

    std::vector<char> CreateSPIRVVector(const char *asset_name);

    void InitializeResources();
};
}